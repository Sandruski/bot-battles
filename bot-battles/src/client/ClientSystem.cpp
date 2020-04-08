#include "ClientSystem.h"

#include "ClientComponent.h"
#include "DeliveryManagerClient.h"
#include "FSM.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "MessageTypes.h"
#include "ReplicationManagerClient.h"
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

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    clientComponent.m_socketAddress = SocketAddress::CreateIPv4(clientComponent.m_ip.c_str(), clientComponent.m_port.c_str());
    assert(clientComponent.m_socketAddress != nullptr);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::SEND_HELLO: {
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        SendHelloPacket(clientComponent);
        break;
    }

    case EventType::SEND_REHELLO: {
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        SendReHelloPacket(clientComponent);
        break;
    }

    case EventType::SEND_BYE: {
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        SendByePacket(clientComponent);
        break;
    }

    case EventType::CONNECT_SOCKETS: {
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        ConnectSockets(clientComponent);
        break;
    }

    case EventType::CHECK_CONNECT: {
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        CheckConnect(clientComponent);
        break;
    }

    case EventType::DISCONNECT_SOCKETS: {
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        DisconnectSockets(clientComponent);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveIncomingPackets(ClientComponent& clientComponent)
{
    InputMemoryStream packet;
    U32 byteCapacity = packet.GetByteCapacity();

    // TCP
    if (clientComponent.m_TCPSocket != nullptr) {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(clientComponent.m_TCPSocket->GetSocket(), &readSet);

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        int iResult = select(0, &readSet, nullptr, nullptr, &timeout);
        if (iResult != 0 && iResult != SOCKET_ERROR) {
            if (FD_ISSET(clientComponent.m_TCPSocket->GetSocket(), &readSet)) {
                I32 readByteCount = clientComponent.m_TCPSocket->Receive(packet.GetPtr(), byteCapacity);
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
    if (clientComponent.m_UDPSocket != nullptr) {
        U32 receivedPacketCount = 0;
        while (receivedPacketCount < MAX_PACKETS_PER_FRAME) {
            SocketAddress fromSocketAddress;
            I32 readByteCount = clientComponent.m_UDPSocket->ReceiveFrom(packet.GetPtr(), byteCapacity, fromSocketAddress);
            if (readByteCount > 0) {
                packet.SetCapacity(readByteCount);
                packet.ResetHead();
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
        GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
        std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
        if (!currentState.expired() && clientComponent.m_entity < INVALID_ENTITY) {
            F32 timeDiff = MyTime::GetInstance().GetTime() - clientComponent.m_lastPacketTime;
            if (timeDiff >= DISCONNECT_TIMEOUT) {
                ConnectionReset(clientComponent);
                DisconnectSockets(clientComponent);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::SendOutgoingPackets(ClientComponent& clientComponent)
{
    if (clientComponent.m_UDPSocket != nullptr) {
        if (!clientComponent.m_inputBuffer.IsEmpty()) {
            U32 index = clientComponent.m_inputBuffer.m_front;
            bool isFound = false;
            while (index < clientComponent.m_inputBuffer.m_back) {
                const Input& input = clientComponent.m_inputBuffer.Get(index);
                if (input.GetFrame() == clientComponent.m_lastAckdFrame) {
                    isFound = true;
                    break;
                }
                ++index;
            }
            if (isFound) {
                clientComponent.m_inputBuffer.Remove(index);
            }
        }

        // Only Gameplay
        GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
        std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
        if (!currentState.expired()) {
            SendInputPacket(clientComponent);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceivePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream)
{
    ServerMessageType type;
    inputStream.Read(type);

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
        clientComponent.m_lastPacketTime = MyTime::GetInstance().GetStartFrameTime();
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveWelcomePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream)
{
    // Only Connect
    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    std::weak_ptr<State> currentState = mainMenuComponent.m_fsm.GetCurrentState();
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
    const bool isConnected = clientComponent.IsConnected();
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

    inputStream.Read(clientComponent.m_playerID);
    assert(clientComponent.m_playerID < INVALID_PLAYER_ID);
    inputStream.Read(clientComponent.m_map);

    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    inputStream.Read(scoreboardComponent.m_gameCount);

    Event newEvent;
    newEvent.eventType = EventType::WELCOME_RECEIVED;
    NotifyEvent(newEvent);

    newEvent.eventType = EventType::PLAYER_ADDED;
    NotifyEvent(newEvent);

    ILOG("Player %s %u has joined the game", clientComponent.m_name.c_str(), clientComponent.m_playerID);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveUnWelcomePacket(ClientComponent& /*clientComponent*/, InputMemoryStream& /*inputStream*/)
{
    // Only Connect
    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    std::weak_ptr<State> currentState = mainMenuComponent.m_fsm.GetCurrentState();
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
void ClientSystem::ReceiveReWelcomePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream)
{
    // Only Restart
    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    std::weak_ptr<State> currentState = scoreboardComponent.m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Restart") {
        U32 gameCount = 0;
        inputStream.Read(gameCount);
        ELOG("ReWelcome packet received but skipped because at incorrect state");
        return;
    }
    const bool isConnected = clientComponent.IsConnected();
    if (!isConnected) {
        U32 gameCount = 0;
        inputStream.Read(gameCount);
        ELOG("ReWelcome packet received but skipped because player is not connected");
        return;
    }

    ILOG("ReWelcome packet received");

    inputStream.Read(scoreboardComponent.m_gameCount);

    Event newEvent;
    newEvent.eventType = EventType::REWELCOME_RECEIVED;
    NotifyEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceivePlayPacket(ClientComponent& clientComponent, InputMemoryStream& /*inputStream*/)
{
    // Only Start
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Start") {
        ELOG("Play packet received but skipped because at incorrect state");
        return;
    }
    const bool isConnected = clientComponent.IsConnected();
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
void ClientSystem::ReceiveResultsPacket(ClientComponent& clientComponent, InputMemoryStream& /*inputStream*/)
{
    // Only Play
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Play") {
        ELOG("Results packet received but skipped because at incorrect state");
        return;
    }
    const bool isConnected = clientComponent.IsConnected();
    if (!isConnected) {
        ELOG("Results packet received but skipped because player is not connected");
        return;
    }

    ILOG("Results packet received");

    Event newEvent;
    newEvent.eventType = EventType::RESULTS_RECEIVED;
    NotifyEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveByePacket(ClientComponent& clientComponent, InputMemoryStream& /*inputStream*/)
{
    // All except Setup
    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    std::weak_ptr<State> currentState = mainMenuComponent.m_fsm.GetCurrentState();
    if (!currentState.expired() && currentState.lock()->GetName() == "Setup") {
        ELOG("Bye packet received but skipped because at incorrect state");
        return;
    }
    const bool isConnected = clientComponent.IsConnected();
    if (!isConnected) {
        ELOG("Bye packet received but skipped because Player is not connected");
        return;
    }

    ILOG("Bye packet received");

    Disconnect(clientComponent);
    DisconnectSockets(clientComponent);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveStatePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream) const
{
    // Only Gameplay
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        ELOG("State packet received but skipped because at incorrect state");
        return;
    }
    const bool isConnected = clientComponent.IsConnected();
    if (!isConnected) {
        ELOG("State packet received but skipped because player is not connected");
        return;
    }
    const bool isValid = clientComponent.m_deliveryManager.ReadState(inputStream);
    if (!isValid) {
        ELOG("State packet received but skipped because it is not valid");
        return;
    }

    ILOG("State packet received");

    F32 timestamp = 0.0f;
    inputStream.Read(timestamp);
    clientComponent.m_RTT = MyTime::GetInstance().GetTime() - timestamp;

    inputStream.Read(clientComponent.m_lastAckdFrame);

    clientComponent.m_replicationManager.Read(inputStream);
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendHelloPacket(const ClientComponent& clientComponent) const
{
    OutputMemoryStream helloPacket;
    helloPacket.Write(ClientMessageType::HELLO);
    helloPacket.Write(clientComponent.m_name);

    bool result = SendTCPPacket(clientComponent, helloPacket);
    if (result) {
        ILOG("Hello packet of length %u successfully sent to server", helloPacket.GetByteLength());
    } else {
        ELOG("Hello packet unsuccessfully sent to server");
    }

    return result;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendReHelloPacket(const ClientComponent& clientComponent) const
{
    OutputMemoryStream reHelloPacket;
    reHelloPacket.Write(ClientMessageType::REHELLO);
    reHelloPacket.Write(clientComponent.m_playerID);

    bool result = SendTCPPacket(clientComponent, reHelloPacket);
    if (result) {
        ILOG("ReHello packet of length %u successfully sent to server", reHelloPacket.GetByteLength());
    } else {
        ELOG("ReHello packet unsuccessfully sent to server");
    }

    return result;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendByePacket(ClientComponent& clientComponent)
{
    OutputMemoryStream byePacket;
    byePacket.Write(ClientMessageType::BYE);
    byePacket.Write(clientComponent.m_playerID);

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
bool ClientSystem::SendInputPacket(ClientComponent& clientComponent) const
{
    OutputMemoryStream inputPacket;
    inputPacket.Write(ClientMessageType::INPUT);
    inputPacket.Write(clientComponent.m_playerID);

    clientComponent.m_deliveryManager.WriteState(inputPacket);

    F32 timestamp = MyTime::GetInstance().GetTime();
    inputPacket.Write(timestamp);

    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    inputPacket.Write(scoreboardComponent.m_gameCount);

    const bool hasInputs = !clientComponent.m_inputBuffer.IsEmpty();
    inputPacket.Write(hasInputs);
    if (hasInputs) {
        U32 inputCount = clientComponent.m_inputBuffer.Count();
        U32 back = clientComponent.m_inputBuffer.m_back;
        if (inputCount > MAX_INPUTS_PER_PACKET) {
            back -= (inputCount - MAX_INPUTS_PER_PACKET);
            inputCount = MAX_INPUTS_PER_PACKET;
        }
        inputPacket.Write(inputCount);
        for (U32 i = clientComponent.m_inputBuffer.m_front; i < back; ++i) {
            const Input& input = clientComponent.m_inputBuffer.Get(i);
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
bool ClientSystem::SendUDPPacket(const ClientComponent& clientComponent, const OutputMemoryStream& outputStream) const
{
    return clientComponent.m_UDPSocket->SendTo(outputStream.GetPtr(), outputStream.GetByteLength(), *clientComponent.m_socketAddress);
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendTCPPacket(const ClientComponent& clientComponent, const OutputMemoryStream& outputStream) const
{
    return clientComponent.m_TCPSocket->Send(outputStream.GetPtr(), outputStream.GetByteLength());
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::ConnectSockets(ClientComponent& clientComponent)
{
    bool ret = true;

    clientComponent.m_TCPSocket = TCPSocket::CreateIPv4();
    assert(clientComponent.m_TCPSocket != nullptr);
    ret = clientComponent.m_TCPSocket->SetReuseAddress(true);
    if (!ret) {
        return ret;
    }
    ret = clientComponent.m_TCPSocket->SetNonBlockingMode(true);
    if (!ret) {
        return ret;
    }
    ret = clientComponent.m_TCPSocket->SetNoDelay(true);
    if (!ret) {
        return ret;
    }
    clientComponent.m_TCPSocket->Connect(*clientComponent.m_socketAddress);
    clientComponent.m_connectTimer.Start();

    clientComponent.m_UDPSocket = UDPSocket::CreateIPv4();
    assert(clientComponent.m_UDPSocket != nullptr);
    ret = clientComponent.m_UDPSocket->SetReuseAddress(true);
    if (!ret) {
        return ret;
    }
    ret = clientComponent.m_UDPSocket->SetNonBlockingMode(true);
    if (!ret) {
        return ret;
    }
    SocketAddress socketAddress = clientComponent.m_TCPSocket->GetLocalSocketAddress();
    ret = clientComponent.m_UDPSocket->Bind(socketAddress);
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::CheckConnect(ClientComponent& clientComponent)
{
    F32 connectCurrentTime = static_cast<F32>(clientComponent.m_connectTimer.ReadSec());
    if (connectCurrentTime >= SECONDS_BETWEEN_CONNECTS) {
        clientComponent.m_TCPSocket->Connect(*clientComponent.m_socketAddress);

        clientComponent.m_connectTimer.Start();
    }

    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(clientComponent.m_TCPSocket->GetSocket(), &writeSet);

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
bool ClientSystem::DisconnectSockets(ClientComponent& clientComponent)
{
    clientComponent.m_UDPSocket = nullptr;
    clientComponent.m_TCPSocket = nullptr;

    ILOG("Sockets disconnected");

    return true;
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ConnectionReset(ClientComponent& clientComponent)
{
    Disconnect(clientComponent);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::Disconnect(ClientComponent& clientComponent)
{
    ILOG("Local player disconnected...");

    clientComponent.Reset();
    clientComponent.m_playerID = INVALID_PLAYER_ID;

    Event newEvent;
    newEvent.eventType = EventType::PLAYER_REMOVED;
    newEvent.networking.entity = clientComponent.m_entity;
    NotifyEvent(newEvent);

    clientComponent.m_entity = INVALID_ENTITY;
}
}
