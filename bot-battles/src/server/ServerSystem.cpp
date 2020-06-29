#include "ServerSystem.h"

#include "BotComponent.h"
#include "ClientProxy.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "DeliveryManagerServer.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "LinkingContext.h"
#include "MainMenuComponent.h"
#include "MessageTypes.h"
#include "ReplicationManagerServer.h"
#include "ReplicationResultManager.h"
#include "ScoreboardComponent.h"
#include "ServerComponent.h"
#include "SightComponent.h"
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

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    serverComponent.lock()->m_incomingPacketsTimer.Start();
    serverComponent.lock()->m_outgoingPacketsTimer.Start();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::ShutDown()
{
    int iResult = WSACleanup();
    if (iResult == SOCKET_ERROR) {
        NETLOG("WSACleanup");
        return false;
    }

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

    case EventType::SEEN_NEW_ENTITY: {
        OnSeenNewEntity(event.sight.playerID, event.sight.seenEntity);
        break;
    }

    case EventType::SEEN_LOST_ENTITY: {
        OnSeenLostEntity(event.sight.playerID, event.sight.seenEntity);
        break;
    }

    case EventType::SEND_WELCOME: {
        std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
        PlayerID playerID = event.networking.playerID;
        std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
        SendWelcomePacket(serverComponent, playerID, clientProxy.lock());
        break;
    }

    case EventType::SEND_REWELCOME: {
        std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
        PlayerID playerID = event.networking.playerID;
        std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
        SendReWelcomePacket(serverComponent, playerID, clientProxy.lock());
        break;
    }

    case EventType::SEND_PLAY: {
        std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
        const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.lock()->GetPlayerIDToClientProxyMap();
        for (const auto& pair : playerIDToClientProxy) {
            PlayerID playerID = pair.first;
            std::shared_ptr<ClientProxy> clientProxy = pair.second;
            SendPlayPacket(serverComponent, playerID, clientProxy);
        }
        break;
    }

    case EventType::SEND_RESULTS: {
        std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
        const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.lock()->GetPlayerIDToClientProxyMap();
        for (const auto& pair : playerIDToClientProxy) {
            PlayerID playerID = pair.first;
            std::shared_ptr<ClientProxy> clientProxy = pair.second;
            SendResultsPacket(serverComponent, playerID, clientProxy);
        }
        break;
    }

    case EventType::SEND_BYE: {
        std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
        const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>> playerIDToClientProxy = serverComponent.lock()->GetPlayerIDToClientProxyMap();
        for (const auto& pair : playerIDToClientProxy) {
            PlayerID playerID = pair.first;
            std::shared_ptr<ClientProxy> clientProxy = pair.second;
            SendByePacket(serverComponent, playerID, clientProxy);
        }
        DisconnectSockets(serverComponent);
        break;
    }

    case EventType::CONNECT_SOCKETS: {
        std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
        ConnectSockets(serverComponent);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveIncomingPackets(std::weak_ptr<ServerComponent> serverComponent)
{
    OPTICK_EVENT();

    F32 incomingPacketCurrentTime = static_cast<F32>(serverComponent.lock()->m_incomingPacketsTimer.ReadSec());
    if (incomingPacketCurrentTime < serverComponent.lock()->m_incomingPacketsTimeout) {
        return;
    }

    serverComponent.lock()->m_incomingPacketsTimer.Start();

    // TCP
    if (!serverComponent.lock()->m_TCPListenSocket.expired()) {
        fd_set readSet;
        FD_ZERO(&readSet);
        for (const auto& TCPSock : serverComponent.lock()->m_TCPSockets) {
            FD_SET(TCPSock->GetSocket(), &readSet);
        }

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        int iResult = select(0, &readSet, nullptr, nullptr, &timeout);
        if (iResult != 0 && iResult != SOCKET_ERROR) {
            for (U32 i = 0; i < serverComponent.lock()->m_TCPSockets.size(); ++i) {
                std::shared_ptr<TCPSocket> TCPSock = serverComponent.lock()->m_TCPSockets.at(i);
                if (FD_ISSET(TCPSock->GetSocket(), &readSet)) {
                    if (TCPSock == serverComponent.lock()->m_TCPListenSocket.lock()) {
                        SocketAddress fromSocketAddress;
                        std::shared_ptr<TCPSocket> acceptedTCPSock = TCPSock->Accept(fromSocketAddress);
                        bool result = acceptedTCPSock->SetReuseAddress(true);
                        if (result) {
                            result = acceptedTCPSock->SetNonBlockingMode(true);
                            if (result) {
                                result = acceptedTCPSock->SetNoDelay(true);
                                if (result) {
                                    serverComponent.lock()->m_TCPSockets.emplace_back(acceptedTCPSock);
                                    ILOG("TCP socket added");
                                }
                            }
                        }
                    } else {
                        InputMemoryStream packet;
                        I32 readByteCount = TCPSock->Receive(packet.GetPtr(), packet.GetByteCapacity());
                        if (readByteCount > 0) {
                            packet.SetCapacity(readByteCount);
                            packet.ResetHead();
                            U32 remainingReadByteCount = readByteCount;
                            U32 previousByteCount = 0;
                            while (remainingReadByteCount > 0) {
                                ReceivePacket(serverComponent, packet, TCPSock->GetRemoteSocketAddress());
                                U32 byteCount = packet.GetByteLength();
                                U32 newByteCount = byteCount - previousByteCount;
                                previousByteCount = byteCount;
                                remainingReadByteCount -= newByteCount;
                                U32 bitCount = BYTES_TO_BITS(byteCount);
                                packet.SetHead(bitCount);
                            }
                        } else if (readByteCount == -WSAECONNRESET || readByteCount == 0) {
                            ConnectionReset(serverComponent, TCPSock->GetRemoteSocketAddress());
                        } else if (readByteCount == -WSAEWOULDBLOCK) {
                            continue;
                        } else {
                            assert(false);
                        }
                    }
                }
            }
        }
    }

    // UDP
    if (serverComponent.lock()->m_UDPSocket != nullptr) {
        U32 receivedPacketCount = 0;
        while (receivedPacketCount < serverComponent.lock()->m_maxPacketsPerFrame) {
            SocketAddress fromSocketAddress;
            InputMemoryStream packet;
            I32 readByteCount = serverComponent.lock()->m_UDPSocket->ReceiveFrom(packet.GetPtr(), packet.GetByteCapacity(), fromSocketAddress);
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

        // Only Gameplay
        std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
        std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
        if (!currentState.expired()) {
            std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>> disconnections;
            const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.lock()->GetPlayerIDToClientProxyMap();
            for (const auto& pair : playerIDToClientProxy) {
                PlayerID playerID = pair.first;
                std::shared_ptr<ClientProxy> clientProxy = pair.second;
                Entity entity = serverComponent.lock()->GetEntity(playerID);
                if (entity < INVALID_ENTITY) {
                    F32 timeDiff = MyTime::GetInstance().GetTime() - clientProxy->m_lastPacketTime;
                    if (timeDiff >= serverComponent.lock()->m_disconnectTimeout) {
                        disconnections.insert(std::make_pair(playerID, clientProxy));
                        continue;
                    }
                }
            }

            for (const auto& disconnection : disconnections) {
                PlayerID playerID = disconnection.first;
                Entity entity = serverComponent.lock()->GetEntity(playerID);
                Disconnect(serverComponent, playerID, entity);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendOutgoingPackets(std::weak_ptr<ServerComponent> serverComponent)
{
    OPTICK_EVENT();

    F32 outgoingPacketCurrentTime = static_cast<F32>(serverComponent.lock()->m_outgoingPacketsTimer.ReadSec());
    if (outgoingPacketCurrentTime < serverComponent.lock()->m_outgoingPacketsTimeout) {
        return;
    }

    serverComponent.lock()->m_outgoingPacketsTimer.Start();

    if (serverComponent.lock()->m_UDPSocket != nullptr) {
        const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.lock()->GetPlayerIDToClientProxyMap();
        for (const auto& pair : playerIDToClientProxy) {
            PlayerID playerID = pair.first;
            std::shared_ptr<ClientProxy> clientProxy = pair.second;

            // Only Gameplay
            std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
            std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
            if (!currentState.expired()) {
                clientProxy->m_deliveryManager.ProcessTimedOutPackets();
                SendStatePacket(serverComponent, playerID, clientProxy);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceivePacket(std::weak_ptr<ServerComponent> serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress)
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

    if (type < ClientMessageType::COUNT) {
        std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
        if (!clientProxy.expired()) {
            clientProxy.lock()->UpdateLastPacketTime();
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveHelloPacket(std::weak_ptr<ServerComponent> serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress, PlayerID& playerID)
{
    playerID = serverComponent.lock()->GetPlayerID(fromSocketAddress);

    // Only Gameplay
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired()) {
        std::string name;
        inputStream.Read(name);
        ELOG("Hello packet received but skipped because at incorrect state");
        return;
    }
    std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
    if (!clientProxy.expired()) {
        std::string name;
        inputStream.Read(name);
        ELOG("Hello packet received but skipped because existing player %u %s", playerID, clientProxy.lock()->GetName());
        return;
    }

    std::string name;
    inputStream.Read(name);
    ILOG("Hello packet received from new player %s", name.c_str());

    playerID = serverComponent.lock()->AddPlayer(fromSocketAddress, name.c_str());
    if (playerID < INVALID_PLAYER_ID) {
        Event newEvent;
        newEvent.eventType = EventType::HELLO_RECEIVED;
        newEvent.networking.playerID = playerID;
        NotifyEvent(newEvent);

        newEvent.eventType = EventType::PLAYER_ADDED;
        NotifyEvent(newEvent);

        ILOG("New player %u %s has joined the game", playerID, name.c_str());
    } else {
        SendUnWelcomePacket(serverComponent, fromSocketAddress);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveReHelloPacket(std::weak_ptr<ServerComponent> serverComponent, InputMemoryStream& inputStream, PlayerID& playerID) const
{
    inputStream.Read(playerID);

    // Only Start
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Start") {
        ELOG("ReHello packet received but skipped because at incorrect state");
        return;
    }
    std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
    if (clientProxy.expired()) {
        ELOG("ReHello packet received but skipped because unknown player");
        return;
    }

    ILOG("ReHello packet received from player %u %s", playerID, clientProxy.lock()->GetName());

    Event newEvent;
    newEvent.eventType = EventType::REHELLO_RECEIVED;
    newEvent.networking.playerID = playerID;
    NotifyEvent(newEvent);

    newEvent.eventType = EventType::PLAYER_ADDED;
    NotifyEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveByePacket(std::weak_ptr<ServerComponent> serverComponent, InputMemoryStream& inputStream, PlayerID& playerID)
{
    inputStream.Read(playerID);

    // All except Setup
    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameServer->GetMainMenuComponent();
    std::weak_ptr<State> currentState = mainMenuComponent.lock()->m_fsm.GetCurrentState();
    if (!currentState.expired() && currentState.lock()->GetName() == "Setup") {
        ELOG("Bye packet received but skipped because at incorrect state");
        return;
    }
    std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
    if (clientProxy.expired()) {
        ELOG("Bye packet received because unknown player");
        return;
    }

    ILOG("Bye packet received from player %u %s", playerID, clientProxy.lock()->GetName());

    Entity entity = serverComponent.lock()->GetEntity(playerID);
    Disconnect(serverComponent, playerID, entity);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveInputPacket(std::weak_ptr<ServerComponent> serverComponent, InputMemoryStream& inputStream, PlayerID& playerID) const
{
    inputStream.Read(playerID);

    // Only Gameplay
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired()) {
        ELOG("Input packet received but skipped because at incorrect state");
        return;
    }
    std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
    if (clientProxy.expired()) {
        ELOG("Input packet received but skipped because unknown player");
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
    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameServer->GetScoreboardComponent();
    if (scoreboardComponent.lock()->m_gameCount != gameCount) {
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
            if (input.GetFrame() > clientProxy.lock()->m_lastAckdFrame) {
                clientProxy.lock()->m_inputBuffer.Add(input);
                clientProxy.lock()->m_lastAckdFrame = input.GetFrame();
                ILOG("Server received ackd frame %u", clientProxy.lock()->m_lastAckdFrame);
            }
            --inputCount;
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendWelcomePacket(const std::weak_ptr<ServerComponent> serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const
{
    OutputMemoryStream welcomePacket;
    welcomePacket.Write(ServerMessageType::WELCOME);

    welcomePacket.Write(playerID);
    welcomePacket.Write(serverComponent.lock()->m_map);
    welcomePacket.Write(serverComponent.lock()->m_duration);

    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameServer->GetScoreboardComponent();
    welcomePacket.Write(scoreboardComponent.lock()->m_gameCount);

    const bool result = SendTCPPacket(serverComponent, welcomePacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("Welcome packet of length %u successfully sent to player %u", welcomePacket.GetByteLength(), playerID);
    } else {
        ELOG("Welcome packet unsuccessfully sent to player %u", playerID);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendUnWelcomePacket(std::weak_ptr<ServerComponent> serverComponent, const SocketAddress& toSocketAddress) const
{
    OutputMemoryStream unWelcomePacket;
    unWelcomePacket.Write(ServerMessageType::UNWELCOME);

    const bool result = SendTCPPacket(serverComponent, unWelcomePacket, toSocketAddress);
    if (result) {
        ILOG("UnWelcome packet of length %u successfully sent to socket address %s", unWelcomePacket.GetByteLength(), toSocketAddress.GetName());
    } else {
        ELOG("UnWelcome packet unsuccessfully sent to socket address %s", toSocketAddress.GetName());
    }

    serverComponent.lock()->RemoveTCPSocket(toSocketAddress);
    ILOG("TCP socket removed");
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendReWelcomePacket(const std::weak_ptr<ServerComponent> serverComponent, PlayerID /*playerID*/, std::shared_ptr<ClientProxy> clientProxy) const
{
    OutputMemoryStream reWelcomePacket;
    reWelcomePacket.Write(ServerMessageType::REWELCOME);

    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameServer->GetScoreboardComponent();
    reWelcomePacket.Write(scoreboardComponent.lock()->m_gameCount);

    const bool result = SendTCPPacket(serverComponent, reWelcomePacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("ReWelcome packet of length %u successfully sent to player %s", reWelcomePacket.GetByteLength(), clientProxy->GetName());
    } else {
        ELOG("ReWelcome packet unsuccessfully sent to player %s", clientProxy->GetName());
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendPlayPacket(const std::weak_ptr<ServerComponent> serverComponent, PlayerID /*playerID*/, std::shared_ptr<ClientProxy> clientProxy) const
{
    OutputMemoryStream playPacket;
    playPacket.Write(ServerMessageType::PLAY);

    const bool result = SendTCPPacket(serverComponent, playPacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("Play packet of length %u successfully sent to player %s", playPacket.GetByteLength(), clientProxy->GetName());
    } else {
        ELOG("Play packet unsuccessfully sent to player %s", clientProxy->GetName());
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendResultsPacket(const std::weak_ptr<ServerComponent> serverComponent, PlayerID /*playerID*/, std::shared_ptr<ClientProxy> clientProxy) const
{
    OutputMemoryStream resultsPacket;
    resultsPacket.Write(ServerMessageType::RESULTS);

    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameServer->GetScoreboardComponent();
    resultsPacket.Write(scoreboardComponent.lock()->m_winnerPlayerID);
    resultsPacket.Write(clientProxy->m_damageInflicted);
    resultsPacket.Write(clientProxy->m_damageReceived);

    const bool result = SendTCPPacket(serverComponent, resultsPacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("Results packet of length %u successfully sent to player %s", resultsPacket.GetByteLength(), clientProxy->GetName());
    } else {
        ELOG("Results packet unsuccessfully sent to player %s", clientProxy->GetName());
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendByePacket(std::weak_ptr<ServerComponent> serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy)
{
    OutputMemoryStream byePacket;
    byePacket.Write(ServerMessageType::BYE);

    const bool result = SendTCPPacket(serverComponent, byePacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("Bye packet of length %u successfully sent to player %s", byePacket.GetByteLength(), clientProxy->GetName());
    } else {
        ELOG("Bye packet unsuccessfully sent to player %s", clientProxy->GetName());
    }

    Entity entity = serverComponent.lock()->GetEntity(playerID);
    Disconnect(serverComponent, playerID, entity);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendStatePacket(const std::weak_ptr<ServerComponent> serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const
{
    const bool hasEntity = serverComponent.lock()->GetEntity(playerID) < INVALID_ENTITY;
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

    if (serverComponent.lock()->m_duration > 0.0f) {
        statePacket.Write(static_cast<F32>(serverComponent.lock()->m_durationTimer.ReadSec()));
    }

    clientProxy->m_replicationManager->Write(statePacket, *delivery.m_replicationResultManager);

    const bool result = SendUDPPacket(serverComponent, statePacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("State packet of length %u successfully sent to player %s", statePacket.GetByteLength(), clientProxy->GetName());
    } else {
        ELOG("State packet unsuccessfully sent to player %s", clientProxy->GetName());
    }
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::SendUDPPacket(const std::weak_ptr<ServerComponent> serverComponent, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const
{
    if (serverComponent.lock()->m_UDPSocket == nullptr) {
        return false;
    }

    I32 bytesSent = serverComponent.lock()->m_UDPSocket->SendTo(outputStream.GetPtr(), outputStream.GetByteLength(), toSocketAddress);
    return (bytesSent == static_cast<I32>(outputStream.GetByteLength()));
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::SendTCPPacket(const std::weak_ptr<ServerComponent> serverComponent, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const
{
    std::weak_ptr<TCPSocket> TCPSock = serverComponent.lock()->GetTCPSocket(toSocketAddress);
    if (TCPSock.expired()) {
        return false;
    }

    I32 bytesSent = TCPSock.lock()->Send(outputStream.GetPtr(), outputStream.GetByteLength());
    return (bytesSent == static_cast<I32>(outputStream.GetByteLength()));
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::ConnectSockets(std::weak_ptr<ServerComponent> serverComponent)
{
    bool ret = true;

    serverComponent.lock()->m_socketAddress = SocketAddress::CreateIPv4(INADDR_ANY, static_cast<U16>(atoi(serverComponent.lock()->m_port.c_str())));
    assert(serverComponent.lock()->m_socketAddress != nullptr);

    serverComponent.lock()->m_UDPSocket = UDPSocket::CreateIPv4();
    assert(serverComponent.lock()->m_UDPSocket != nullptr);
    ret = serverComponent.lock()->m_UDPSocket->SetReuseAddress(true);
    if (!ret) {
        return ret;
    }
    ret = serverComponent.lock()->m_UDPSocket->SetNonBlockingMode(true);
    if (!ret) {
        return ret;
    }
    ret = serverComponent.lock()->m_UDPSocket->Bind(*serverComponent.lock()->m_socketAddress);
    if (!ret) {
        return ret;
    }

    std::shared_ptr<TCPSocket> TCPListenSocket = TCPSocket::CreateIPv4();
    assert(TCPListenSocket != nullptr);
    ret = TCPListenSocket->SetReuseAddress(true);
    if (!ret) {
        return ret;
    }
    ret = TCPListenSocket->SetNonBlockingMode(true);
    if (!ret) {
        return ret;
    }
    ret = TCPListenSocket->SetNoDelay(true);
    if (!ret) {
        return ret;
    }
    ret = TCPListenSocket->Bind(*serverComponent.lock()->m_socketAddress);
    if (!ret) {
        return ret;
    }
    ret = TCPListenSocket->Listen(MAX_PLAYER_IDS);
    if (!ret) {
        return ret;
    }
    serverComponent.lock()->m_TCPListenSocket = serverComponent.lock()->m_TCPSockets.emplace_back(TCPListenSocket);

    ILOG("Connect sockets");
    ILOG("Sockets connected");

    Event newEvent;
    newEvent.eventType = EventType::SOCKETS_CONNECTED;
    NotifyEvent(newEvent);

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::DisconnectSockets(std::weak_ptr<ServerComponent> serverComponent)
{
    serverComponent.lock()->m_UDPSocket = nullptr;
    serverComponent.lock()->m_TCPSockets.clear();

    ILOG("Disconnect sockets");
    ILOG("Sockets disconnected");

    return true;
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ConnectionReset(std::weak_ptr<ServerComponent> serverComponent, const SocketAddress& socketAddress)
{
    PlayerID playerID = serverComponent.lock()->GetPlayerID(socketAddress);
    Entity entity = serverComponent.lock()->GetEntity(playerID);
    Disconnect(serverComponent, playerID, entity);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::Disconnect(std::weak_ptr<ServerComponent> serverComponent, PlayerID playerID, Entity entity)
{
    std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
    if (clientProxy.expired()) {
        return;
    }

    ILOG("Player disconnected...");

    serverComponent.lock()->RemoveTCPSocket(clientProxy.lock()->GetSocketAddress());
    ILOG("TCP socket removed");

    clientProxy.lock()->Reset();
    serverComponent.lock()->RemovePlayer(playerID);

    Event newEvent;
    newEvent.eventType = EventType::PLAYER_REMOVED;
    newEvent.networking.entity = entity;
    NotifyEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnPlayerAdded(PlayerID playerID) const
{
    assert(playerID < INVALID_PLAYER_ID);

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
    const std::unordered_map<NetworkID, Entity>& networkIDToEntity = g_gameServer->GetLinkingContext().GetNetworkIDToEntityMap();
    for (const auto& pair : networkIDToEntity) {
        clientProxy.lock()->m_replicationManager->AddCommand(pair.first, static_cast<U64>(ComponentMemberType::ALL), false);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnNetworkEntityAdded(NetworkID networkID) const
{
    assert(networkID < INVALID_NETWORK_ID);

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.lock()->GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        std::shared_ptr<ClientProxy> clientProxy = pair.second;
        clientProxy->m_replicationManager->AddCommand(networkID, static_cast<U64>(ComponentMemberType::ALL), false);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnNetworkEntityRemoved(NetworkID networkID) const
{
    assert(networkID < INVALID_NETWORK_ID);

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.lock()->GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        std::shared_ptr<ClientProxy> clientProxy = pair.second;
        clientProxy->m_replicationManager->SetIsReplicated(networkID, true);
        clientProxy->m_replicationManager->SetRemove(networkID);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnComponentMemberChanged(U64 dirtyState, Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
    NetworkID networkID = linkingContext.GetNetworkID(entity);
    if (networkID >= INVALID_NETWORK_ID) {
        return;
    }

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.lock()->GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        pair.second->m_replicationManager->AddDirtyState(networkID, dirtyState);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnSeenNewEntity(PlayerID playerID, Entity seenEntity) const
{
    assert(playerID < INVALID_PLAYER_ID && seenEntity < INVALID_ENTITY);

    LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
    NetworkID networkID = linkingContext.GetNetworkID(seenEntity);
    if (networkID >= INVALID_NETWORK_ID) {
        return;
    }

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
    clientProxy.lock()->m_replicationManager->SetIsReplicated(networkID, true);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnSeenLostEntity(PlayerID playerID, Entity seenEntity) const
{
    assert(playerID < INVALID_PLAYER_ID && seenEntity < INVALID_ENTITY);

    LinkingContext& linkingContext = g_gameServer->GetLinkingContext();
    NetworkID networkID = linkingContext.GetNetworkID(seenEntity);
    if (networkID >= INVALID_NETWORK_ID) {
        return;
    }

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
    clientProxy.lock()->m_replicationManager->SetIsReplicated(networkID, false);
}
}
