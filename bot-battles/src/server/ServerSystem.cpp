#include "ServerSystem.h"

#include "ClientProxy.h"
#include "ComponentMemberTypes.h"
#include "DeliveryManagerServer.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "LinkingContext.h"
#include "MessageTypes.h"
#include "ReplicationManagerServer.h"
#include "ReplicationResultManager.h"
#include "ServerComponent.h"
#include "TCPSocket.h"
#include "UDPSocket.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::PLAYER_ADDED: {
        OnPlayerAdded(event.networking.playerID);
        break;
    }

    case EventType::NETWORK_ENTITY_ADDED: {
        OnNetworkEntityAdded(event.networking.networkID);
        break;
    }

    case EventType::NETWORK_ENTITY_REMOVED: {
        OnNetworkEntityRemoved(event.networking.networkID);
        break;
    }

    case EventType::COMPONENT_MEMBER_CHANGED: {
        OnComponentMemberChanged(event.component.dirtyState, event.component.entity);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::StartUp()
{
    WORD winsockVersion = MAKEWORD(2, 2);
    WSADATA winsockData;
    int iResult = WSAStartup(winsockVersion, &winsockData);
    if (iResult == SOCKET_ERROR) {
        NETLOG("WSAStartup");
        return false;
    }

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    serverComponent.m_socketAddress = SocketAddress::CreateIPv4(INADDR_ANY, static_cast<U16>(atoi(serverComponent.m_port.c_str())));
    assert(serverComponent.m_socketAddress != nullptr);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::ConnectSockets(ServerComponent& serverComponent)
{
    bool ret = false;

    serverComponent.m_UDPSocket = UDPSocket::CreateIPv4();
    assert(serverComponent.m_UDPSocket != nullptr);
    ret = serverComponent.m_UDPSocket->SetReuseAddress(true);
    if (!ret) {
        return ret;
    }
    ret = serverComponent.m_UDPSocket->SetNonBlockingMode(true);
    if (!ret) {
        return ret;
    }
    ret = serverComponent.m_UDPSocket->Bind(*serverComponent.m_socketAddress);
    if (!ret) {
        return ret;
    }

    serverComponent.m_TCPListenSocket = TCPSocket::CreateIPv4();
    assert(serverComponent.m_TCPListenSocket != nullptr);
    ret = serverComponent.m_TCPListenSocket->SetReuseAddress(true);
    if (!ret) {
        return ret;
    }
    ret = serverComponent.m_TCPListenSocket->SetNonBlockingMode(true);
    if (!ret) {
        return ret;
    }
    ret = serverComponent.m_TCPListenSocket->SetNoDelay(true);
    if (!ret) {
        return ret;
    }
    ret = serverComponent.m_TCPListenSocket->Bind(*serverComponent.m_socketAddress);
    if (!ret) {
        return ret;
    }
    ret = serverComponent.m_TCPListenSocket->Listen(MAX_PLAYER_IDS);
    if (!ret) {
        return ret;
    }
    serverComponent.m_TCPSockets.emplace_back(serverComponent.m_TCPListenSocket);

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::DisconnectSockets(ServerComponent& serverComponent)
{
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        PlayerID playerID = pair.first;
        Entity entity = serverComponent.GetEntity(playerID);
        Disconnect(serverComponent, playerID, entity);
    }

    // TODO: send bye packet

    serverComponent.m_UDPSocket = nullptr;
    serverComponent.m_TCPSockets.clear();
    serverComponent.m_TCPListenSocket = nullptr;

    return true;
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveIncomingPackets(ServerComponent& serverComponent)
{
    InputMemoryStream packet;
    U32 byteCapacity = packet.GetByteCapacity();

    // TCP
    if (serverComponent.m_TCPListenSocket != nullptr) {
        fd_set readSet;
        FD_ZERO(&readSet);
        for (const auto& TCPSock : serverComponent.m_TCPSockets) {
            FD_SET(TCPSock->GetSocket(), &readSet);
        }

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        int iResult = select(0, &readSet, nullptr, nullptr, &timeout);
        if (iResult != 0 && iResult != SOCKET_ERROR) {
            for (U32 i = 0; i < serverComponent.m_TCPSockets.size(); ++i) {
                std::shared_ptr<TCPSocket> TCPSock = serverComponent.m_TCPSockets.at(i);
                if (FD_ISSET(TCPSock->GetSocket(), &readSet)) {
                    if (TCPSock == serverComponent.m_TCPListenSocket) {
                        SocketAddress fromSocketAddress;
                        std::shared_ptr<TCPSocket> acceptedTCPSock = TCPSock->Accept(fromSocketAddress);
                        bool result = acceptedTCPSock->SetReuseAddress(true);
                        if (result) {
                            result = acceptedTCPSock->SetNonBlockingMode(true);
                            if (result) {
                                result = acceptedTCPSock->SetNoDelay(true);
                                if (result) {
                                    serverComponent.m_TCPSockets.emplace_back(acceptedTCPSock);
                                    ILOG("TCP socket added");
                                }
                            }
                        }
                    } else {
                        I32 readByteCount = TCPSock->Receive(packet.GetPtr(), byteCapacity);
                        if (readByteCount > 0) {
                            packet.SetCapacity(readByteCount);
                            packet.ResetHead();
                            ReceivePacket(serverComponent, packet, TCPSock->GetRemoteSocketAddress());
                        } else if (readByteCount == -WSAECONNRESET || readByteCount == 0) {
                            ConnectionReset(serverComponent, TCPSock->GetRemoteSocketAddress());
                        } else if (readByteCount == -WSAEWOULDBLOCK) {
                            continue;
                        }
                    }
                }
            }
        }
    }

    // UDP
    if (serverComponent.m_UDPSocket != nullptr) {
        U32 receivedPacketCount = 0;
        while (receivedPacketCount < MAX_PACKETS_PER_FRAME) {
            SocketAddress fromSocketAddress;
            I32 readByteCount = serverComponent.m_UDPSocket->ReceiveFrom(packet.GetPtr(), byteCapacity, fromSocketAddress);
            if (readByteCount > 0) {
                packet.SetCapacity(readByteCount);
                packet.ResetHead();
                ReceivePacket(serverComponent, packet, fromSocketAddress);
                ++receivedPacketCount;
            } else if (readByteCount == -WSAECONNRESET || readByteCount == 0) {
                ConnectionReset(serverComponent, fromSocketAddress);
            } else if (readByteCount == -WSAEWOULDBLOCK) {
                break;
            }
        }

        GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
        if (gameplayComponent.m_phase == GameplayComponent::GameplayPhase::PLAY) {
            std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>> disconnections;
            const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
            for (const auto& pair : playerIDToClientProxy) {
                PlayerID playerID = pair.first;
                std::shared_ptr<ClientProxy> clientProxy = pair.second;

                F32 timeDiff = MyTime::GetInstance().GetTime() - clientProxy->GetLastPacketTime();
                if (timeDiff >= DISCONNECT_TIMEOUT) {
                    disconnections.insert(std::make_pair(playerID, clientProxy));
                    continue;
                }
            }

            for (const auto& disconnection : disconnections) {
                PlayerID playerID = disconnection.first;
                Entity entity = serverComponent.GetEntity(playerID);
                Disconnect(serverComponent, playerID, entity);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendOutgoingPackets(ServerComponent& serverComponent)
{
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        PlayerID playerID = pair.first;
        std::shared_ptr<ClientProxy> clientProxy = pair.second;

        if (serverComponent.m_TCPListenSocket != nullptr) {
            if (clientProxy->m_sendWelcomePacket) {
                SendWelcomePacket(serverComponent, playerID, clientProxy);
                clientProxy->m_sendWelcomePacket = false;
            }
            if (clientProxy->m_sendReWelcomePacket) {
                SendReWelcomePacket(serverComponent, playerID, clientProxy);
                clientProxy->m_sendReWelcomePacket = false;
            }
            if (clientProxy->m_sendResultPacket) {
                SendResultPacket(serverComponent, playerID, clientProxy);
                clientProxy->m_sendResultPacket = false;
            }
        }

        if (serverComponent.m_UDPSocket != nullptr) {
            GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
            if (gameplayComponent.m_phase != GameplayComponent::GameplayPhase::NONE) {
                clientProxy->m_deliveryManager.ProcessTimedOutPackets();
                SendStatePacket(serverComponent, playerID, clientProxy);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceivePacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress)
{
    ClientMessageType type;
    inputStream.Read(type);
    PlayerID playerID = INVALID_PLAYER_ID;

    switch (type) {

    case ClientMessageType::HELLO: {
        ReceiveHelloPacket(serverComponent, inputStream, fromSocketAddress, playerID);
        break;
    }

    case ClientMessageType::REHELLO: {
        ReceiveReHelloPacket(serverComponent, inputStream, playerID);
        break;
    }

    case ClientMessageType::INPUT: {
        ReceiveInputPacket(serverComponent, inputStream, playerID);
        break;
    }

    case ClientMessageType::BYE: {
        ReceiveByePacket(serverComponent, inputStream, playerID);
        break;
    }

    default: {
        WLOG("Unknown packet received from socket address %s", fromSocketAddress.GetName());
        break;
    }
    }

    if (playerID < INVALID_PLAYER_ID && type < ClientMessageType::COUNT) {
        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        if (!clientProxy.expired()) {
            clientProxy.lock()->UpdateLastPacketTime();
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveHelloPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress, PlayerID& playerID)
{
    playerID = serverComponent.GetPlayerID(fromSocketAddress);
    if (playerID < INVALID_PLAYER_ID) {
        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        ELOG("Hello packet received from existing player %u %s", playerID, clientProxy.lock()->GetName());
        return;
    }

    std::string name;
    inputStream.Read(name);

    ILOG("Hello packet received from new player %s", name.c_str());

    playerID = serverComponent.AddPlayer(fromSocketAddress, name.c_str());
    if (playerID < INVALID_PLAYER_ID) {
        Event newEvent;
        newEvent.eventType = EventType::PLAYER_ADDED;
        newEvent.networking.playerID = playerID;
        NotifyEvent(newEvent);

        ILOG("New player %u %s has joined the game", playerID, name.c_str());

        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        clientProxy.lock()->m_sendWelcomePacket = true;
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveReHelloPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, PlayerID& playerID) const
{
    inputStream.Read(playerID);
    if (playerID >= INVALID_PLAYER_ID) {
        ELOG("ReHello packet received from unknown player");
        return;
    }

    std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
    if (clientProxy.expired()) {
        return;
    }

    Event newEvent;
    newEvent.eventType = EventType::PLAYER_ADDED;
    newEvent.networking.playerID = playerID;
    NotifyEvent(newEvent);

    ILOG("ReHello packet received from player %u %s", playerID, clientProxy.lock()->GetName());

    clientProxy.lock()->m_sendReWelcomePacket = true;
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveInputPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, PlayerID& playerID) const
{
    inputStream.Read(playerID);
    if (playerID >= INVALID_PLAYER_ID) {
        ELOG("Input packet received from unknown player");
        return;
    }

    std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
    if (clientProxy.expired()) {
        return;
    }

    const bool isValid = clientProxy.lock()->m_deliveryManager.ReadState(inputStream);
    if (!isValid) {
        ELOG("Input packet received but skipped because it is not valid");
        return;
    }

    ILOG("Input packet received from player %u %s", playerID, clientProxy.lock()->GetName());

    inputStream.Read(clientProxy.lock()->m_timestamp);

    U32 gameCount = 0;
    inputStream.Read(gameCount);
    ScoreboardComponent& scoreboardComponent = g_gameServer->GetScoreboardComponent();
    if (scoreboardComponent.m_gameCount != gameCount) {
        ELOG("Input packet received from another game");
        return;
    }

    bool hasInputs = false;
    inputStream.Read(hasInputs);
    if (hasInputs) {
        U32 inputCount = 0;
        inputStream.Read(inputCount);
        while (inputCount > 0) {
            Input input;
            input.Read(inputStream);
            if (input.GetFrame() > clientProxy.lock()->m_lastAckdFrame) { // TODO: be careful if new frame is 15 and last frame is 13 and frame 14 contains a shoot for example
                clientProxy.lock()->m_inputBuffer.Add(input);
                clientProxy.lock()->m_lastAckdFrame = input.GetFrame();
                ILOG("Server received ackd frame %u", clientProxy.lock()->m_lastAckdFrame);
            }
            --inputCount;
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveByePacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, PlayerID& playerID)
{
    inputStream.Read(playerID);
    if (playerID >= INVALID_PLAYER_ID) {
        ELOG("Bye packet received from unknown player");
        return;
    }

    std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
    if (clientProxy.expired()) {
        return;
    }

    ILOG("Bye packet received from player %u %s", playerID, clientProxy.lock()->GetName());

    Entity entity = serverComponent.GetEntity(playerID);
    Disconnect(serverComponent, playerID, entity);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendWelcomePacket(const ServerComponent& serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const
{
    OutputMemoryStream welcomePacket;
    welcomePacket.Write(ServerMessageType::WELCOME);
    const bool isSuccessful = playerID < INVALID_PLAYER_ID;
    welcomePacket.Write(isSuccessful);
    if (isSuccessful) {
        welcomePacket.Write(playerID);
        welcomePacket.Write(serverComponent.m_map);

        GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
        welcomePacket.Write(gameplayComponent.m_phase);
    }

    const bool result = SendTCPPacket(serverComponent, welcomePacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("Welcome packet of length %u successfully sent to player %u", welcomePacket.GetByteLength(), playerID);
    } else {
        ELOG("Welcome packet of length %u unsuccessfully sent to player %u", welcomePacket.GetByteLength(), playerID);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendReWelcomePacket(const ServerComponent& serverComponent, PlayerID /*playerID*/, std::shared_ptr<ClientProxy> clientProxy) const
{
    OutputMemoryStream reWelcomePacket;
    reWelcomePacket.Write(ServerMessageType::REWELCOME);

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    reWelcomePacket.Write(gameplayComponent.m_phase);

    const char* name = clientProxy->GetName();
    const bool result = SendTCPPacket(serverComponent, reWelcomePacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("ReWelcome packet of length %u successfully sent to player %s", reWelcomePacket.GetByteLength(), name);
    } else {
        ELOG("ReWelcome packet of length %u unsuccessfully sent to player %s", reWelcomePacket.GetByteLength(), name);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendStatePacket(const ServerComponent& serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const
{
    const bool hasEntity = serverComponent.GetEntity(playerID) < INVALID_ENTITY;
    if (!hasEntity) {
        ILOG("State packet not sent because entity is not created");
        return;
    }

    OutputMemoryStream statePacket;
    statePacket.Write(ServerMessageType::STATE);

    Delivery& delivery = clientProxy->m_deliveryManager.WriteState(statePacket);
    delivery.m_replicationResultManager = std::make_shared<ReplicationResultManager>(std::weak_ptr<ReplicationManagerServer>(clientProxy->m_replicationManager));

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    statePacket.Write(gameplayComponent.m_phase);

    statePacket.Write(clientProxy->m_timestamp);

    statePacket.Write(clientProxy->m_lastAckdFrame);

    clientProxy->m_replicationManager->Write(statePacket, *delivery.m_replicationResultManager);

    const char* name = clientProxy->GetName();
    const bool result = SendUDPPacket(serverComponent, statePacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("State packet of length %u successfully sent to player %s", statePacket.GetByteLength(), name);
    } else {
        ELOG("State packet of length %u unsuccessfully sent to player %s", statePacket.GetByteLength(), name);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendResultPacket(const ServerComponent& serverComponent, PlayerID /*playerID*/, std::shared_ptr<ClientProxy> clientProxy) const
{
    OutputMemoryStream resultPacket;
    resultPacket.Write(ServerMessageType::RESULT);

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    resultPacket.Write(gameplayComponent.m_phase);

    ScoreboardComponent& scoreboardComponent = g_gameServer->GetScoreboardComponent();
    resultPacket.Write(scoreboardComponent.m_gameCount);

    const char* name = clientProxy->GetName();
    const bool result = SendTCPPacket(serverComponent, resultPacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("Result packet of length %u successfully sent to player %s", resultPacket.GetByteLength(), name);
    } else {
        ELOG("Result packet of length %u unsuccessfully sent to player %s", resultPacket.GetByteLength(), name);
    }
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::SendUDPPacket(const ServerComponent& serverComponent, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const
{
    return serverComponent.m_UDPSocket->SendTo(outputStream.GetPtr(), outputStream.GetByteLength(), toSocketAddress);
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::SendTCPPacket(const ServerComponent& serverComponent, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const
{
    std::weak_ptr<TCPSocket> TCPSock = serverComponent.GetTCPSocket(toSocketAddress);
    return TCPSock.lock()->Send(outputStream.GetPtr(), outputStream.GetByteLength());
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ConnectionReset(ServerComponent& serverComponent, const SocketAddress& socketAddress)
{
    PlayerID playerID = serverComponent.GetPlayerID(socketAddress);
    Entity entity = serverComponent.GetEntity(playerID);
    Disconnect(serverComponent, playerID, entity);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::Disconnect(ServerComponent& serverComponent, PlayerID playerID, Entity entity)
{
    std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
    if (clientProxy.expired()) {
        return;
    }

    serverComponent.RemoveTCPSocket(clientProxy.lock()->GetSocketAddress());
    ILOG("TCP socket removed");

    clientProxy.lock()->Reset();
    serverComponent.RemovePlayer(playerID);

    Event newEvent;
    newEvent.eventType = EventType::PLAYER_REMOVED;
    newEvent.networking.entity = entity;
    NotifyEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnPlayerAdded(PlayerID playerID) const
{
    assert(playerID < INVALID_PLAYER_ID);

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
    const std::unordered_map<NetworkID, Entity>& networkIDToEntity = g_gameServer->GetLinkingContext().GetNetworkIDToEntityMap();
    for (const auto& pair : networkIDToEntity) {
        clientProxy.lock()->m_replicationManager->AddCommand(pair.first, static_cast<U32>(ComponentMemberType::ALL));
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnNetworkEntityAdded(NetworkID networkID) const
{
    assert(networkID < INVALID_NETWORK_ID);

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        pair.second->m_replicationManager->AddCommand(networkID, static_cast<U32>(ComponentMemberType::ALL));
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnNetworkEntityRemoved(NetworkID networkID) const
{
    assert(networkID < INVALID_NETWORK_ID);

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        pair.second->m_replicationManager->SetRemove(networkID);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnComponentMemberChanged(U32 dirtyState, Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    NetworkID networkID = g_gameServer->GetLinkingContext().GetNetworkID(entity);
    if (networkID >= INVALID_NETWORK_ID) {
        return;
    }

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        pair.second->m_replicationManager->AddDirtyState(networkID, dirtyState);
    }
}
}
