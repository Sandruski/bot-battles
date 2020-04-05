#include "ServerSystem.h"

#include "ClientProxy.h"
#include "ComponentMemberTypes.h"
#include "DeliveryManagerServer.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameServer.h"
#include "LinkingContext.h"
#include "MessageTypes.h"
#include "ReplicationManagerServer.h"
#include "ReplicationResultManager.h"
#include "ServerComponent.h"
#include "State.h"
#include "TCPSocket.h"
#include "UDPSocket.h"

namespace sand {

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

    case EventType::SEND_WELCOME: {
        ServerComponent& serverComponent = g_gameServer->GetServerComponent();
        PlayerID playerID = event.networking.playerID;
        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        SendWelcomePacket(serverComponent, playerID, clientProxy.lock());
        break;
    }

    case EventType::SEND_REWELCOME: {
        ServerComponent& serverComponent = g_gameServer->GetServerComponent();
        PlayerID playerID = event.networking.playerID;
        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        SendReWelcomePacket(serverComponent, playerID, clientProxy.lock());
        break;
    }

    case EventType::SEND_PLAY: {
        ServerComponent& serverComponent = g_gameServer->GetServerComponent();
        const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
        for (const auto& pair : playerIDToClientProxy) {
            PlayerID playerID = pair.first;
            std::shared_ptr<ClientProxy> clientProxy = pair.second;
            SendPlayPacket(serverComponent, playerID, clientProxy);
        }
        break;
    }

    case EventType::SEND_RESULTS: {
        ServerComponent& serverComponent = g_gameServer->GetServerComponent();
        const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
        for (const auto& pair : playerIDToClientProxy) {
            PlayerID playerID = pair.first;
            std::shared_ptr<ClientProxy> clientProxy = pair.second;
            SendResultsPacket(serverComponent, playerID, clientProxy);
        }
        break;
    }

    case EventType::SEND_BYE: {
        ServerComponent& serverComponent = g_gameServer->GetServerComponent();
        const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
        for (const auto& pair : playerIDToClientProxy) {
            PlayerID playerID = pair.first;
            std::shared_ptr<ClientProxy> clientProxy = pair.second;
            SendByePacket(serverComponent, playerID, clientProxy);
        }
        DisconnectSockets(serverComponent);
        break;
    }

    case EventType::CONNECT_SOCKETS: {
        ServerComponent& serverComponent = g_gameServer->GetServerComponent();
        ConnectSockets(serverComponent);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveIncomingPackets(ServerComponent& serverComponent)
{
    InputMemoryStream packet;
    U32 byteCapacity = packet.GetByteCapacity();

    // TCP
    if (!serverComponent.m_TCPListenSocket.expired()) {
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
                    if (TCPSock == serverComponent.m_TCPListenSocket.lock()) {
                        SocketAddress fromSocketAddress;
                        std::shared_ptr<TCPSocket> acceptedTCPSock = TCPSock->Accept(fromSocketAddress);
                        bool result = acceptedTCPSock->SetReuseAddress(true);
                        if (result) {
                            //result = acceptedTCPSock->SetNonBlockingMode(true);
                            //if (result) {
                            //result = acceptedTCPSock->SetNoDelay(true);
                            //if (result) {
                            serverComponent.m_TCPSockets.emplace_back(acceptedTCPSock);
                            ILOG("TCP socket added");
                            //}
                            //}
                        }
                    } else {
                        I32 readByteCount = TCPSock->Receive(packet.GetPtr(), byteCapacity);
                        if (readByteCount > 0) {
                            packet.SetCapacity(readByteCount);
                            packet.ResetHead();
                            U32 previousByteCount = 0;
                            while (readByteCount > 0) {
                                ReceivePacket(serverComponent, packet, TCPSock->GetRemoteSocketAddress());
                                U32 byteCount = packet.GetByteLength();
                                U32 newByteCount = byteCount - previousByteCount;
                                previousByteCount = newByteCount;
                                readByteCount -= newByteCount;
                                U32 bitCount = BYTES_TO_BITS(byteCount);
                                packet.SetHead(bitCount);
                            }
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

        //GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
        //if (gameplayComponent.m_phase == GameplayComponent::GameplayPhase::PLAY) {
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
        //}
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendOutgoingPackets(ServerComponent& serverComponent)
{
    if (serverComponent.m_UDPSocket != nullptr) {
        const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
        for (const auto& pair : playerIDToClientProxy) {
            PlayerID playerID = pair.first;
            std::shared_ptr<ClientProxy> clientProxy = pair.second;

            //GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
            //if (gameplayComponent.m_phase != GameplayComponent::GameplayPhase::NONE) {
            clientProxy->m_deliveryManager.ProcessTimedOutPackets();
            SendStatePacket(serverComponent, playerID, clientProxy);
            //}
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

    case ClientMessageType::BYE: {
        ReceiveByePacket(serverComponent, inputStream, playerID);
        break;
    }

    case ClientMessageType::INPUT: {
        ReceiveInputPacket(serverComponent, inputStream, playerID);
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
    // Only Start
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    playerID = serverComponent.GetPlayerID(fromSocketAddress);
    if (currentState.expired() || currentState.lock()->GetName() != "Start" || playerID < INVALID_PLAYER_ID) {
        std::string name;
        inputStream.Read(name);
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
        newEvent.eventType = EventType::HELLO_RECEIVED;
        newEvent.networking.playerID = playerID;
        NotifyEvent(newEvent);

        newEvent.eventType = EventType::PLAYER_ADDED;
        NotifyEvent(newEvent);

        ILOG("New player %u %s has joined the game", playerID, name.c_str());
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveReHelloPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, PlayerID& playerID) const
{
    // Only Start
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Start") {
        return;
    }

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
    newEvent.eventType = EventType::REHELLO_RECEIVED;
    newEvent.networking.playerID = playerID;
    NotifyEvent(newEvent);

    newEvent.eventType = EventType::PLAYER_ADDED;
    NotifyEvent(newEvent);

    ILOG("ReHello packet received from player %u %s", playerID, clientProxy.lock()->GetName());
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveByePacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, PlayerID& playerID)
{
    // All except Setup
    MainMenuComponent& mainMenuComponent = g_gameServer->GetMainMenuComponent();
    std::weak_ptr<State> currentState = mainMenuComponent.m_fsm.GetCurrentState();
    if (!currentState.expired() && currentState.lock()->GetName() == "Setup") {
        return;
    }

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
void ServerSystem::ReceiveInputPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, PlayerID& playerID) const
{
    // Only Play
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Play") {
        return;
    }

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
void ServerSystem::SendWelcomePacket(const ServerComponent& serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const
{
    OutputMemoryStream welcomePacket;
    welcomePacket.Write(ServerMessageType::WELCOME);

    const bool isSuccessful = playerID < INVALID_PLAYER_ID;
    welcomePacket.Write(isSuccessful);
    if (isSuccessful) {
        welcomePacket.Write(playerID);
        welcomePacket.Write(serverComponent.m_map);
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

    const char* name = clientProxy->GetName();
    const bool result = SendTCPPacket(serverComponent, reWelcomePacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("ReWelcome packet of length %u successfully sent to player %s", reWelcomePacket.GetByteLength(), name);
    } else {
        ELOG("ReWelcome packet of length %u unsuccessfully sent to player %s", reWelcomePacket.GetByteLength(), name);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendPlayPacket(const ServerComponent& serverComponent, PlayerID /*playerID*/, std::shared_ptr<ClientProxy> clientProxy) const
{
    OutputMemoryStream playPacket;
    playPacket.Write(ServerMessageType::PLAY);

    const char* name = clientProxy->GetName();
    const bool result = SendTCPPacket(serverComponent, playPacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("Play packet of length %u successfully sent to player %s", playPacket.GetByteLength(), name);
    } else {
        ELOG("Play packet of length %u unsuccessfully sent to player %s", playPacket.GetByteLength(), name);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendResultsPacket(const ServerComponent& serverComponent, PlayerID /*playerID*/, std::shared_ptr<ClientProxy> clientProxy) const
{
    OutputMemoryStream resultsPacket;
    resultsPacket.Write(ServerMessageType::RESULTS);

    ScoreboardComponent& scoreboardComponent = g_gameServer->GetScoreboardComponent();
    resultsPacket.Write(scoreboardComponent.m_gameCount);

    const char* name = clientProxy->GetName();
    const bool result = SendTCPPacket(serverComponent, resultsPacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("Results packet of length %u successfully sent to player %s", resultsPacket.GetByteLength(), name);
    } else {
        ELOG("Results packet of length %u unsuccessfully sent to player %s", resultsPacket.GetByteLength(), name);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendByePacket(ServerComponent& serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy)
{
    OutputMemoryStream byePacket;
    byePacket.Write(ServerMessageType::BYE);

    const char* name = clientProxy->GetName();
    const bool result = SendTCPPacket(serverComponent, byePacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("Bye packet of length %u successfully sent to player %s", byePacket.GetByteLength(), name);
    } else {
        ELOG("Bye packet of length %u unsuccessfully sent to player %s", byePacket.GetByteLength(), name);
    }

    Entity entity = serverComponent.GetEntity(playerID);
    Disconnect(serverComponent, playerID, entity);
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
bool ServerSystem::SendUDPPacket(const ServerComponent& serverComponent, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const
{
    I32 bytesSent = serverComponent.m_UDPSocket->SendTo(outputStream.GetPtr(), outputStream.GetByteLength(), toSocketAddress);
    return (bytesSent == static_cast<I32>(outputStream.GetByteLength()));
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::SendTCPPacket(const ServerComponent& serverComponent, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const
{
    std::weak_ptr<TCPSocket> TCPSock = serverComponent.GetTCPSocket(toSocketAddress);
    I32 bytesSent = TCPSock.lock()->Send(outputStream.GetPtr(), outputStream.GetByteLength());
    ILOG("TCP sent %i bytes", bytesSent);
    return (bytesSent == static_cast<I32>(outputStream.GetByteLength()));
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::ConnectSockets(ServerComponent& serverComponent)
{
    bool ret = true;

    if (!serverComponent.m_TCPListenSocket.expired() && serverComponent.m_UDPSocket != nullptr) {
        return ret;
    }

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

    std::shared_ptr<TCPSocket> TCPListenSocket = TCPSocket::CreateIPv4();
    assert(TCPListenSocket != nullptr);
    ret = TCPListenSocket->SetReuseAddress(true);
    if (!ret) {
        return ret;
    } /*
    ret = TCPListenSocket->SetNonBlockingMode(true);
    if (!ret) {
        return ret;
    }*/
    /*
    ret = TCPListenSocket->SetNoDelay(true);
    if (!ret) {
        return ret;
    }*/
    ret = TCPListenSocket->Bind(*serverComponent.m_socketAddress);
    if (!ret) {
        return ret;
    }
    ret = TCPListenSocket->Listen(MAX_PLAYER_IDS);
    if (!ret) {
        return ret;
    }
    serverComponent.m_TCPListenSocket = serverComponent.m_TCPSockets.emplace_back(TCPListenSocket);

    Event newEvent;
    newEvent.eventType = EventType::SOCKETS_CONNECTED;
    NotifyEvent(newEvent);

    ILOG("Sockets connected");

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::DisconnectSockets(ServerComponent& serverComponent)
{
    if (!serverComponent.m_TCPListenSocket.expired() && serverComponent.m_UDPSocket != nullptr) {
        return true;
    }

    serverComponent.m_UDPSocket = nullptr;
    serverComponent.m_TCPSockets.clear();

    ILOG("Sockets disconnected");

    return true;
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

    ILOG("Remote player disconnected...");

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
