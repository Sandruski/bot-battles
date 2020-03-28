#include "ClientSystem.h"

#include "ClientComponent.h"
#include "DeliveryManagerClient.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "MessageTypes.h"
#include "ReplicationManagerClient.h"
#include "SocketAddress.h"
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
bool ClientSystem::Connect(ClientComponent& clientComponent)
{
    bool ret = false;

    if (clientComponent.m_connect) {
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
        ret = clientComponent.m_TCPSocket->Connect(*clientComponent.m_socketAddress);
        if (!ret) {
            return ret;
        }

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

        clientComponent.m_sendHelloPacket = true;
        clientComponent.m_connect = false;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveIncomingPackets(ClientComponent& clientComponent)
{
    if (clientComponent.m_connect) {
        return;
    }

    InputMemoryStream packet;
    U32 byteCapacity = packet.GetByteCapacity();
    U32 receivedPacketCount = 0;

    // TCP
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(clientComponent.m_TCPSocket->GetSocket(), &readSet);

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    int iResult = select(0, &readSet, nullptr, nullptr, &timeout);
    if (iResult == 0 || iResult == SOCKET_ERROR) {
        NETLOG("select");
    } else if (FD_ISSET(clientComponent.m_TCPSocket->GetSocket(), &readSet)) {
        I32 readByteCount = clientComponent.m_TCPSocket->Receive(packet.GetPtr(), byteCapacity);
        if (readByteCount > 0) {
            packet.SetCapacity(readByteCount);
            packet.ResetHead();
            ReceivePacket(clientComponent, packet);
            ++receivedPacketCount;
        } else if (readByteCount == -WSAECONNRESET) {
            ConnectionReset(clientComponent);
        } else if (readByteCount == 0) {
            // TODO: graceful disconnection if readByteCount == 0?
        }
    }

    // UDP
    while (receivedPacketCount < MAX_PACKETS_PER_FRAME) {
        SocketAddress fromSocketAddress;
        I32 readByteCount = clientComponent.m_UDPSocket->ReceiveFrom(packet.GetPtr(), byteCapacity, fromSocketAddress);
        if (readByteCount > 0) {
            packet.SetCapacity(readByteCount);
            packet.ResetHead();
            ReceivePacket(clientComponent, packet);
            ++receivedPacketCount;
        } else if (readByteCount == -WSAECONNRESET) {
            ConnectionReset(clientComponent);
        } else if (readByteCount == 0 || -WSAEWOULDBLOCK) {
            // TODO: graceful disconnection if readByteCount == 0?
            break;
        }
    }

    // TODO: handle timeout disconnections here
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::SendOutgoingPackets(ClientComponent& clientComponent)
{
    if (clientComponent.m_connect) {
        return;
    }

    clientComponent.m_inputBuffer.Remove(clientComponent.m_lastAckdFrame);

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    if (gameplayComponent.m_phase != GameplayComponent::GameplayPhase::NONE) {
        F32 timeout = MyTime::GetInstance().GetTime() - clientComponent.m_lastPacketTime;
        if (timeout >= DISCONNECT_TIMEOUT) {
            Disconnect(clientComponent);
            return;
        }

        SendInputPacket(clientComponent);
    }

    if (clientComponent.m_sendHelloPacket) {
        SendHelloPacket(clientComponent);
        clientComponent.m_sendHelloPacket = false;
    }
    if (clientComponent.m_sendReHelloPacket) {
        SendReHelloPacket(clientComponent);
        clientComponent.m_sendReHelloPacket = false;
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

    case ServerMessageType::REWELCOME: {
        ReceiveReWelcomePacket(clientComponent, inputStream);
        break;
    }

    case ServerMessageType::STATE: {
        ReceiveStatePacket(clientComponent, inputStream);
        break;
    }

    case ServerMessageType::RESULT: {
        ReceiveResultPacket(clientComponent, inputStream);
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
    const bool isConnected = clientComponent.IsConnected();
    if (isConnected) {
        ELOG("Welcome packet received but skipped because Player is already connected");
        return;
    }

    ILOG("Welcome packet received");

    bool isSuccessful = false;
    inputStream.Read(isSuccessful);
    if (isSuccessful) {
        inputStream.Read(clientComponent.m_playerID);
        assert(clientComponent.m_playerID < INVALID_PLAYER_ID);
        inputStream.Read(clientComponent.m_map);

        GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
        inputStream.Read(gameplayComponent.m_phase);

        Event newEvent;
        newEvent.eventType = EventType::WELCOME_RECEIVED;
        newEvent.networking.playerID = clientComponent.m_playerID;
        NotifyEvent(newEvent);

        newEvent.eventType = EventType::PLAYER_ADDED;
        NotifyEvent(newEvent);

        ILOG("Player %s %u has joined the game", clientComponent.m_name.c_str(), clientComponent.m_playerID);
    } else {
        // TODO
        /*
        Event newEvent;
        newEvent.eventType = EventType::PLAYER_UNWELCOMED;
        NotifyEvent(newEvent);*/
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveReWelcomePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream)
{
    const bool isConnected = clientComponent.IsConnected();
    if (!isConnected) {
        ELOG("ReWelcome packet received but skipped because Player is not connected");
        return;
    }

    ILOG("ReWelcome packet received");

    Event newEvent;
    newEvent.eventType = EventType::REWELCOME_RECEIVED;
    newEvent.networking.playerID = clientComponent.m_playerID;
    NotifyEvent(newEvent);

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    inputStream.Read(gameplayComponent.m_phase);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveStatePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream) const
{
    const bool isConnected = clientComponent.IsConnected();
    if (!isConnected) {
        ELOG("State packet received but skipped because Player is not connected");
        return;
    }

    const bool isValid = clientComponent.m_deliveryManager.ReadState(inputStream);
    if (!isValid) {
        ELOG("State packet received but skipped because it is not valid");
        return;
    }

    ILOG("State packet received");

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    inputStream.Read(gameplayComponent.m_phase);

    F32 timestamp = 0.0f;
    inputStream.Read(timestamp);
    clientComponent.m_RTT = MyTime::GetInstance().GetTime() - timestamp;

    inputStream.Read(clientComponent.m_lastAckdFrame);

    clientComponent.m_replicationManager.Read(inputStream);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveResultPacket(ClientComponent& clientComponent, InputMemoryStream& inputStream)
{
    const bool isConnected = clientComponent.IsConnected();
    if (!isConnected) {
        ELOG("Result packet received but skipped because Player is not connected");
        return;
    }

    ILOG("Result packet received");

    Event newEvent;
    newEvent.eventType = EventType::RESULT_RECEIVED;
    newEvent.networking.playerID = clientComponent.m_playerID;
    NotifyEvent(newEvent);

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    inputStream.Read(gameplayComponent.m_phase);
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
        ELOG("Hello packet of length %u unsuccessfully sent to server", helloPacket.GetByteLength());
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
        ELOG("ReHello packet of length %u unsuccessfully sent to server", reHelloPacket.GetByteLength());
    }

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
        ELOG("Input packet of length %u unsuccessfully sent to server", inputPacket.GetByteLength());
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
void ClientSystem::ConnectionReset(ClientComponent& clientComponent)
{
    Disconnect(clientComponent);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::Disconnect(ClientComponent& clientComponent)
{
    clientComponent.m_playerID = INVALID_PLAYER_ID;
    clientComponent.Reset();

    Event newEvent;
    newEvent.eventType = EventType::PLAYER_REMOVED;
    newEvent.networking.entity = clientComponent.m_entity;
    NotifyEvent(newEvent);
}
}
