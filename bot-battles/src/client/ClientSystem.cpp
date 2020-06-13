#include "ClientSystem.h"

#include "ClientComponent.h"
#include "DeliveryManagerClient.h"
#include "FSM.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "MainMenuComponent.h"
#include "MessageTypes.h"
#include "ReplicationManagerClient.h"
#include "ScoreboardComponent.h"
#include "SocketAddress.h"
#include "State.h"
#include "TCPSocket.h"
#include "UDPSocket.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool ClientSystem::StartUp()
{
    WORD winsockVersion = MAKEWORD(2, 2);
    WSADATA winsockData;
    int iResult = WSAStartup(winsockVersion, &winsockData);
    if (iResult == SOCKET_ERROR) {
        NETLOG("WSAStartup");
        return false;
    }

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    clientComponent.lock()->m_incomingPacketsTimer.Start();
    clientComponent.lock()->m_outgoingPacketsTimer.Start();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::PreUpdate()
{
    NotifyEvents();

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    clientComponent.lock()->m_replicationManager.NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::ShutDown()
{
    int iResult = WSACleanup();
    if (iResult == SOCKET_ERROR) {
        NETLOG("WSACleanup");
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::SEND_HELLO: {
        std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
        SendHelloPacket(clientComponent);
        break;
    }

    case EventType::SEND_REHELLO: {
        std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
        SendReHelloPacket(clientComponent);
        break;
    }

    case EventType::SEND_BYE: {
        std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
        SendByePacket(clientComponent);
        break;
    }

    case EventType::CONNECT_SOCKETS: {
        std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
        ConnectSockets(clientComponent);
        break;
    }

    case EventType::CHECK_CONNECT: {
        std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
        CheckConnect(clientComponent);
        break;
    }

    case EventType::DISCONNECT_SOCKETS: {
        std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
        DisconnectSockets(clientComponent);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveIncomingPackets(std::weak_ptr<ClientComponent> clientComponent)
{
    OPTICK_EVENT();

    F32 incomingPacketsCurrentTime = static_cast<F32>(clientComponent.lock()->m_incomingPacketsTimer.ReadSec());
    if (incomingPacketsCurrentTime < clientComponent.lock()->m_incomingPacketsTimeout) {
        return;
    }

    clientComponent.lock()->m_incomingPacketsTimer.Start();

    // TCP
    if (clientComponent.lock()->m_TCPSocket != nullptr) {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(clientComponent.lock()->m_TCPSocket->GetSocket(), &readSet);

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        int iResult = select(0, &readSet, nullptr, nullptr, &timeout);
        if (iResult != 0 && iResult != SOCKET_ERROR) {
            if (FD_ISSET(clientComponent.lock()->m_TCPSocket->GetSocket(), &readSet)) {
                InputMemoryStream packet;
                I32 readByteCount = clientComponent.lock()->m_TCPSocket->Receive(packet.GetPtr(), packet.GetByteCapacity());
                if (readByteCount > 0) {
                    packet.SetCapacity(readByteCount);
                    packet.ResetHead();
                    U32 remainingReadByteCount = readByteCount;
                    U32 previousByteCount = 0;
                    while (remainingReadByteCount > 0) {
                        ReceivePacket(clientComponent, packet);
                        U32 byteCount = packet.GetByteLength();
                        U32 newByteCount = byteCount - previousByteCount;
                        previousByteCount = byteCount;
                        remainingReadByteCount -= newByteCount;
                        U32 bitCount = BYTES_TO_BITS(byteCount);
                        packet.SetHead(bitCount);
                    }
                } else if (readByteCount == -WSAECONNRESET || readByteCount == 0) {
                    ConnectionReset(clientComponent);
                    DisconnectSockets(clientComponent);
                } else if (readByteCount == -WSAEWOULDBLOCK) {
                }
            }
        }
    }

    // UDP
    if (clientComponent.lock()->m_UDPSocket != nullptr) {
        U32 receivedPacketCount = 0;
        while (receivedPacketCount < clientComponent.lock()->m_maxPacketsPerFrame) {
            SocketAddress fromSocketAddress;
            InputMemoryStream packet;
            I32 readByteCount = clientComponent.lock()->m_UDPSocket->ReceiveFrom(packet.GetPtr(), packet.GetByteCapacity(), fromSocketAddress);
            if (readByteCount > 0) {
                packet.SetCapacity(readByteCount);
                packet.ResetHead();
                ILOG("readByteCount %i", readByteCount);
                ReceivePacket(clientComponent, packet);
                ++receivedPacketCount;
            } else if (readByteCount == -WSAECONNRESET || readByteCount == 0) {
                ConnectionReset(clientComponent);
                DisconnectSockets(clientComponent);
                break;
            } else if (readByteCount == -WSAEWOULDBLOCK) {
                break;
            }
        }

        // Only Gameplay
        std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
        std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
        if (!currentState.expired() && clientComponent.lock()->m_entity < INVALID_ENTITY) {
            F32 timeDiff = MyTime::GetInstance().GetTime() - clientComponent.lock()->m_lastPacketTime;
            if (timeDiff >= clientComponent.lock()->m_disconnectTimeout) {
                ConnectionReset(clientComponent);
                DisconnectSockets(clientComponent);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::SendOutgoingPackets(std::weak_ptr<ClientComponent> clientComponent)
{
    OPTICK_EVENT();

    F32 outgoingPacketsCurrentTime = static_cast<F32>(clientComponent.lock()->m_outgoingPacketsTimer.ReadSec());
    if (outgoingPacketsCurrentTime < clientComponent.lock()->m_outgoingPacketsTimeout) {
        return;
    }

    clientComponent.lock()->m_outgoingPacketsTimer.Start();

    if (clientComponent.lock()->m_UDPSocket != nullptr) {
        // Only Gameplay
        std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
        std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
        if (!currentState.expired()) {
            SendInputPacket(clientComponent);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceivePacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& inputStream)
{
    ServerMessageType type;
    inputStream.Read(type);
    ILOG("Packet size %u", inputStream.GetByteCapacity());

    switch (type) {

    case ServerMessageType::WELCOME: {
        ReceiveWelcomePacket(clientComponent, inputStream);
        break;
    }

    case ServerMessageType::UNWELCOME: {
        ReceiveUnWelcomePacket(clientComponent, inputStream);
        break;
    }

    case ServerMessageType::REWELCOME: {
        ReceiveReWelcomePacket(clientComponent, inputStream);
        break;
    }

    case ServerMessageType::PLAY: {
        ReceivePlayPacket(clientComponent, inputStream);
        break;
    }

    case ServerMessageType::RESULTS: {
        ReceiveResultsPacket(clientComponent, inputStream);
        break;
    }

    case ServerMessageType::BYE: {
        ReceiveByePacket(clientComponent, inputStream);
        break;
    }

    case ServerMessageType::STATE: {
        ReceiveStatePacket(clientComponent, inputStream);
        break;
    }

    default: {
        WLOG("Unknown packet received");
        break;
    }
    }

    if (type < ServerMessageType::COUNT) {
        clientComponent.lock()->m_lastPacketTime = MyTime::GetInstance().GetStartFrameTime();
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveWelcomePacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& inputStream)
{
    // Only Connect
    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameClient->GetMainMenuComponent();
    std::weak_ptr<State> currentState = mainMenuComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Connect") {
        PlayerID playerID = INVALID_PLAYER_ID;
        inputStream.Read(playerID);
        std::string map;
        inputStream.Read(map);
        U32 gameCount = 0;
        inputStream.Read(gameCount);
        ELOG("Welcome packet received but skipped because at incorrect state");
        return;
    }
    const bool isConnected = clientComponent.lock()->IsConnected();
    if (isConnected) {
        PlayerID playerID = INVALID_PLAYER_ID;
        inputStream.Read(playerID);
        std::string map;
        inputStream.Read(map);
        U32 gameCount = 0;
        inputStream.Read(gameCount);
        ELOG("Welcome packet received but skipped because player is already connected");
        return;
    }

    ILOG("Welcome packet received");

    inputStream.Read(clientComponent.lock()->m_playerID);
    assert(clientComponent.lock()->m_playerID < INVALID_PLAYER_ID);
    inputStream.Read(clientComponent.lock()->m_map);
    inputStream.Read(clientComponent.lock()->m_duration);

    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameClient->GetScoreboardComponent();
    inputStream.Read(scoreboardComponent.lock()->m_gameCount);

    Event newEvent;
    newEvent.eventType = EventType::WELCOME_RECEIVED;
    NotifyEvent(newEvent);

    newEvent.eventType = EventType::PLAYER_ADDED;
    NotifyEvent(newEvent);

    ILOG("Player %s %u has joined the game", clientComponent.lock()->m_name.c_str(), clientComponent.lock()->m_playerID);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveUnWelcomePacket(std::weak_ptr<ClientComponent> /*clientComponent*/, InputMemoryStream& /*inputStream*/)
{
    // Only Connect
    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameClient->GetMainMenuComponent();
    std::weak_ptr<State> currentState = mainMenuComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Connect") {
        ELOG("UnWelcome packet received but skipped because at incorrect state");
        return;
    }

    ILOG("UnWelcome packet received");

    Event newEvent;
    newEvent.eventType = EventType::UNWELCOME_RECEIVED;
    NotifyEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveReWelcomePacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& inputStream)
{
    // Only Restart
    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameClient->GetScoreboardComponent();
    std::weak_ptr<State> currentState = scoreboardComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Restart") {
        U32 gameCount = 0;
        inputStream.Read(gameCount);
        ELOG("ReWelcome packet received but skipped because at incorrect state");
        return;
    }
    const bool isConnected = clientComponent.lock()->IsConnected();
    if (!isConnected) {
        U32 gameCount = 0;
        inputStream.Read(gameCount);
        ELOG("ReWelcome packet received but skipped because player is not connected");
        return;
    }

    ILOG("ReWelcome packet received");

    inputStream.Read(scoreboardComponent.lock()->m_gameCount);

    Event newEvent;
    newEvent.eventType = EventType::REWELCOME_RECEIVED;
    NotifyEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceivePlayPacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& /*inputStream*/)
{
    // Only Start
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Start") {
        ELOG("Play packet received but skipped because at incorrect state");
        return;
    }
    const bool isConnected = clientComponent.lock()->IsConnected();
    if (!isConnected) {
        ELOG("Play packet received but skipped because player is not connected");
        return;
    }

    ILOG("Play packet received");

    Event newEvent;
    newEvent.eventType = EventType::PLAY_RECEIVED;
    NotifyEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveResultsPacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& inputStream)
{
    // Only Play
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Play") {
        ELOG("Results packet received but skipped because at incorrect state");
        return;
    }
    const bool isConnected = clientComponent.lock()->IsConnected();
    if (!isConnected) {
        ELOG("Results packet received but skipped because player is not connected");
        return;
    }

    ILOG("Results packet received");

    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameClient->GetScoreboardComponent();
    inputStream.Read(scoreboardComponent.lock()->m_winnerPlayerID);
    inputStream.Read(clientComponent.lock()->m_damageInflicted);
    inputStream.Read(clientComponent.lock()->m_damageReceived);

    Event newEvent;
    newEvent.eventType = EventType::RESULTS_RECEIVED;
    NotifyEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveByePacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& /*inputStream*/)
{
    // All except Setup
    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameClient->GetMainMenuComponent();
    std::weak_ptr<State> currentState = mainMenuComponent.lock()->m_fsm.GetCurrentState();
    if (!currentState.expired() && currentState.lock()->GetName() == "Setup") {
        ELOG("Bye packet received but skipped because at incorrect state");
        return;
    }
    const bool isConnected = clientComponent.lock()->IsConnected();
    if (!isConnected) {
        ELOG("Bye packet received but skipped because Player is not connected");
        return;
    }

    ILOG("Bye packet received");

    Disconnect(clientComponent);
    DisconnectSockets(clientComponent);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveStatePacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& inputStream) const
{
    // Only Gameplay
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired()) {
        ELOG("State packet received but skipped because at incorrect state");
        return;
    }
    const bool isConnected = clientComponent.lock()->IsConnected();
    if (!isConnected) {
        ELOG("State packet received but skipped because player is not connected");
        return;
    }
    const bool isValid = clientComponent.lock()->m_deliveryManager.ReadState(inputStream);
    if (!isValid) {
        ELOG("State packet received but skipped because it is not valid");
        return;
    }

    ILOG("State packet received");

    F32 timestamp = 0.0f;
    inputStream.Read(timestamp);
    clientComponent.lock()->m_RTT = MyTime::GetInstance().GetTime() - timestamp;

    inputStream.Read(clientComponent.lock()->m_lastAckdFrame);
    ILOG("Last ackd %u", clientComponent.lock()->m_lastAckdFrame);

    if (clientComponent.lock()->m_duration > 0) {
        inputStream.Read(clientComponent.lock()->m_durationTime);
    }

    Entity entity = clientComponent.lock()->m_entity;
    clientComponent.lock()->m_replicationManager.Read(inputStream);
    if (clientComponent.lock()->m_entity != entity) {
        Event newEvent;
        newEvent.eventType = EventType::PLAYER_ENTITY_ADDED;
        NotifyEvent(newEvent);
    }
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendHelloPacket(const std::weak_ptr<ClientComponent> clientComponent) const
{
    OutputMemoryStream helloPacket;
    helloPacket.Write(ClientMessageType::HELLO);
    helloPacket.Write(clientComponent.lock()->m_name);

    bool result = SendTCPPacket(clientComponent, helloPacket);
    if (result) {
        ILOG("Hello packet of length %u successfully sent to server", helloPacket.GetByteLength());
    } else {
        ELOG("Hello packet unsuccessfully sent to server");
    }

    return result;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendReHelloPacket(const std::weak_ptr<ClientComponent> clientComponent) const
{
    OutputMemoryStream reHelloPacket;
    reHelloPacket.Write(ClientMessageType::REHELLO);
    reHelloPacket.Write(clientComponent.lock()->m_playerID);

    bool result = SendTCPPacket(clientComponent, reHelloPacket);
    if (result) {
        ILOG("ReHello packet of length %u successfully sent to server", reHelloPacket.GetByteLength());
    } else {
        ELOG("ReHello packet unsuccessfully sent to server");
    }

    return result;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendByePacket(std::weak_ptr<ClientComponent> clientComponent)
{
    OutputMemoryStream byePacket;
    byePacket.Write(ClientMessageType::BYE);
    byePacket.Write(clientComponent.lock()->m_playerID);

    bool result = SendTCPPacket(clientComponent, byePacket);
    if (result) {
        ILOG("Bye packet of length %u successfully sent to server", byePacket.GetByteLength());
    } else {
        ELOG("Bye packet unsuccessfully sent to server");
    }

    Disconnect(clientComponent);
    DisconnectSockets(clientComponent);

    return result;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendInputPacket(std::weak_ptr<ClientComponent> clientComponent) const
{
    OutputMemoryStream inputPacket;
    inputPacket.Write(ClientMessageType::INPUT);
    inputPacket.Write(clientComponent.lock()->m_playerID);

    clientComponent.lock()->m_deliveryManager.WriteState(inputPacket);

    F32 timestamp = MyTime::GetInstance().GetTime();
    inputPacket.Write(timestamp);

    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameClient->GetScoreboardComponent();
    inputPacket.Write(scoreboardComponent.lock()->m_gameCount);

    const bool hasInputs = !clientComponent.lock()->m_inputBuffer.IsEmpty();
    inputPacket.Write(hasInputs);
    if (hasInputs) {
        U32 inputCount = clientComponent.lock()->m_inputBuffer.Count();
        U32 back = clientComponent.lock()->m_inputBuffer.m_back;
        if (inputCount > clientComponent.lock()->m_maxInputsPerPaquet) {
            U32 diff = inputCount - clientComponent.lock()->m_maxInputsPerPaquet;
            back -= diff;
            inputCount = clientComponent.lock()->m_maxInputsPerPaquet;
        }
        inputPacket.Write(inputCount);
        for (U32 i = clientComponent.lock()->m_inputBuffer.m_front; i < back; ++i) {
            const Input& input = clientComponent.lock()->m_inputBuffer.Get(i);
            input.Write(inputPacket);
        }
    }

    const bool result = SendUDPPacket(clientComponent, inputPacket);
    if (result) {
        ILOG("Input packet of length %u successfully sent to server", inputPacket.GetByteLength());
    } else {
        ELOG("Input packet unsuccessfully sent to server");
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendUDPPacket(const std::weak_ptr<ClientComponent> clientComponent, const OutputMemoryStream& outputStream) const
{
    return clientComponent.lock()->m_UDPSocket->SendTo(outputStream.GetPtr(), outputStream.GetByteLength(), *clientComponent.lock()->m_socketAddress);
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendTCPPacket(const std::weak_ptr<ClientComponent> clientComponent, const OutputMemoryStream& outputStream) const
{
    return clientComponent.lock()->m_TCPSocket->Send(outputStream.GetPtr(), outputStream.GetByteLength());
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::ConnectSockets(std::weak_ptr<ClientComponent> clientComponent)
{
    bool ret = true;

    clientComponent.lock()->m_socketAddress = SocketAddress::CreateIPv4(clientComponent.lock()->m_ip.c_str(), clientComponent.lock()->m_port.c_str());
    assert(clientComponent.lock()->m_socketAddress != nullptr);

    clientComponent.lock()->m_TCPSocket = TCPSocket::CreateIPv4();
    assert(clientComponent.lock()->m_TCPSocket != nullptr);
    ret = clientComponent.lock()->m_TCPSocket->SetReuseAddress(true);
    if (!ret) {
        return ret;
    }
    ret = clientComponent.lock()->m_TCPSocket->SetNonBlockingMode(true);
    if (!ret) {
        return ret;
    }
    ret = clientComponent.lock()->m_TCPSocket->SetNoDelay(true);
    if (!ret) {
        return ret;
    }
    clientComponent.lock()->m_TCPSocket->Connect(*clientComponent.lock()->m_socketAddress);
    clientComponent.lock()->m_connectTimer.Start();

    clientComponent.lock()->m_UDPSocket = UDPSocket::CreateIPv4();
    assert(clientComponent.lock()->m_UDPSocket != nullptr);
    ret = clientComponent.lock()->m_UDPSocket->SetReuseAddress(true);
    if (!ret) {
        return ret;
    }
    ret = clientComponent.lock()->m_UDPSocket->SetNonBlockingMode(true);
    if (!ret) {
        return ret;
    }
    SocketAddress socketAddress = clientComponent.lock()->m_TCPSocket->GetLocalSocketAddress();
    ret = clientComponent.lock()->m_UDPSocket->Bind(socketAddress);
    if (!ret) {
        return ret;
    }

    ILOG("Connect sockets");

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::CheckConnect(std::weak_ptr<ClientComponent> clientComponent)
{
    if (clientComponent.lock()->m_TCPSocket == nullptr) {
        return false;
    }

    F32 connectCurrentTime = static_cast<F32>(clientComponent.lock()->m_connectTimer.ReadSec());
    if (connectCurrentTime >= clientComponent.lock()->m_secondsBetweenConnect) {
        clientComponent.lock()->m_TCPSocket->Connect(*clientComponent.lock()->m_socketAddress);

        clientComponent.lock()->m_connectTimer.Start();
    }

    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(clientComponent.lock()->m_TCPSocket->GetSocket(), &writeSet);

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    int iResult = select(0, nullptr, &writeSet, nullptr, &timeout);
    if (iResult == 0 || iResult == SOCKET_ERROR) {
        NETLOG("select");
        return false;
    }

    ILOG("Sockets connected");

    Event newEvent;
    newEvent.eventType = EventType::SOCKETS_CONNECTED;
    NotifyEvent(newEvent);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::DisconnectSockets(std::weak_ptr<ClientComponent> clientComponent)
{
    clientComponent.lock()->m_UDPSocket = nullptr;
    clientComponent.lock()->m_TCPSocket = nullptr;

    ILOG("Disconnect sockets");
    ILOG("Sockets disconnected");

    return true;
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ConnectionReset(std::weak_ptr<ClientComponent> clientComponent)
{
    Disconnect(clientComponent);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::Disconnect(std::weak_ptr<ClientComponent> clientComponent)
{
    ILOG("Local player disconnected...");

    clientComponent.lock()->Reset();
    clientComponent.lock()->m_playerID = INVALID_PLAYER_ID;

    Event newEvent;
    newEvent.eventType = EventType::PLAYER_REMOVED;
    newEvent.networking.entity = clientComponent.lock()->m_entity;
    NotifyEvent(newEvent);

    clientComponent.lock()->m_entity = INVALID_ENTITY;
}
}
