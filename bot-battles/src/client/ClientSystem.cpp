#include "ClientSystem.h"

#include "ClientComponent.h"
#include "DeliveryManagerClient.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "MessageTypes.h"
#include "ReplicationManagerClient.h"
#include "SocketAddress.h"
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
    clientComponent.m_socket = UDPSocket::CreateIPv4();
    assert(clientComponent.m_socket != nullptr);
    clientComponent.m_socket->SetNonBlockingMode(true);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveIncomingPackets(ClientComponent& clientComponent)
{
    InputMemoryStream packet;
    U32 byteCapacity = packet.GetByteCapacity();
    SocketAddress fromSocketAddress;

    U32 receivedPacketCount = 0;

    while (receivedPacketCount < MAX_PACKETS_PER_FRAME) {
        I32 readByteCount = clientComponent.m_socket->ReceiveFrom(packet.GetPtr(), byteCapacity, fromSocketAddress);
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
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::SendOutgoingPackets(ClientComponent& clientComponent)
{
    clientComponent.m_inputBuffer.Remove(clientComponent.m_lastAckdFrame);

    const bool isConnected = clientComponent.IsConnected();
    if (isConnected) {
        F32 timeout = MyTime::GetInstance().GetTime() - clientComponent.m_lastPacketTime;
        if (timeout >= DISCONNECT_TIMEOUT) {
            Disconnect(clientComponent);
            return;
        }

        SendInputPacket(clientComponent);
    } else {
        SendHelloPacket(clientComponent);
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
    const bool isConnected = clientComponent.IsConnected();
    if (isConnected) {
        ILOG("Welcome packet received but skipped because Player is already connected");
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
        newEvent.eventType = EventType::PLAYER_ADDED;
        newEvent.networking.playerID = clientComponent.m_playerID;
        NotifyEvent(newEvent);

        ILOG("Player %s %u has joined the game", clientComponent.m_name.c_str(), clientComponent.m_playerID);
    } else {
        // TODO
        Event newEvent;
        newEvent.eventType = EventType::PLAYER_UNWELCOMED;
        NotifyEvent(newEvent);
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveStatePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream) const
{
    const bool isConnected = clientComponent.IsConnected();
    if (!isConnected) {
        ILOG("State packet received but skipped because Player is not connected");
        return;
    }

    const bool isValid = clientComponent.m_deliveryManager.ReadState(inputStream);
    if (!isValid) {
        ILOG("State packet received but skipped because it is not valid");
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
bool ClientSystem::SendHelloPacket(const ClientComponent& clientComponent) const
{
    OutputMemoryStream helloPacket;
    helloPacket.Write(ClientMessageType::HELLO);
    helloPacket.Write(clientComponent.m_name);

    const bool result = SendPacket(clientComponent, helloPacket);
    if (result) {
        ILOG("Hello packet of length %u successfully sent to server", helloPacket.GetByteLength());
    } else {
        ILOG("Hello packet of length %u unsuccessfully sent to server", helloPacket.GetByteLength());
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

    const bool result = SendPacket(clientComponent, inputPacket);
    if (result) {
        ILOG("Input packet of length %u successfully sent to server", inputPacket.GetByteLength());
    } else {
        ILOG("Input packet of length %u unsuccessfully sent to server", inputPacket.GetByteLength());
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendPacket(const ClientComponent& clientComponent, const OutputMemoryStream& outputStream) const
{
    return clientComponent.m_socket->SendTo(outputStream.GetPtr(), outputStream.GetByteLength(), *clientComponent.m_socketAddress);
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
