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
bool ClientSystem::Update()
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();

    ReceiveIncomingPackets(clientComponent);
    SendOutgoingPackets(clientComponent);

    return true;
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::SendOutgoingPackets(ClientComponent& clientComponent) const
{
    const bool isConnected = clientComponent.IsConnected();
    if (!isConnected) {
        SendHelloPacket(clientComponent);
    } else {
        SendInputPacket(clientComponent);
    }
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendHelloPacket(const ClientComponent& clientComponent) const
{
    OutputMemoryStream helloPacket;
    helloPacket.Write(ClientMessageType::HELLO);
    helloPacket.Write(clientComponent.m_name);

    ILOG("Sending hello packet to server...");

    const bool result = SendPacket(clientComponent, helloPacket);
    if (result) {
        ILOG("Hello packet successfully sent to server");
    }

    return result;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendInputPacket(ClientComponent& clientComponent) const
{
    if (!clientComponent.m_moves.HasMoves()) {
        return false;
    }

    OutputMemoryStream inputPacket;
    inputPacket.Write(ClientMessageType::INPUT);
    inputPacket.Write(clientComponent.m_playerID);
    clientComponent.m_deliveryManager.WriteState(inputPacket);

    U32 totalMoveCount = clientComponent.m_moves.GetMoveCount();
    U32 startIndex = totalMoveCount > MAX_MOVES_PER_PACKET ? totalMoveCount - MAX_MOVES_PER_PACKET : 0;
    U32 moveCount = totalMoveCount - startIndex;
    inputPacket.Write(moveCount, GetRequiredBits<MAX_MOVES_PER_PACKET>::value);
    for (U32 i = startIndex; i < totalMoveCount; ++i) {
        const Move& move = clientComponent.m_moves.GetMove(i);
        move.Write(inputPacket);
    }

    ILOG("Sending input packet to server...");

    const bool result = SendPacket(clientComponent, inputPacket);
    if (result) {
        ILOG("Input packet successfully sent to server");
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendPacket(const ClientComponent& clientComponent, const OutputMemoryStream& outputStream) const
{
    return clientComponent.m_socket->SendTo(outputStream.GetPtr(), outputStream.GetByteLength(), *clientComponent.m_socketAddress);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveIncomingPackets(ClientComponent& clientComponent)
{
    InputMemoryStream packet;
    SocketAddress fromSocketAddress;

    U32 receivedPacketCount = 0;

    while (receivedPacketCount < MAX_PACKETS_PER_FRAME) {
        I32 readByteCount = clientComponent.m_socket->ReceiveFrom(packet.GetPtr(), packet.GetByteCapacity(), fromSocketAddress);
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
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveWelcomePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream)
{
    const bool isConnected = clientComponent.IsConnected();
    if (isConnected) {
        ILOG("Welcome packet received but skipped");
        return;
    }

    ILOG("Welcome packet received");

    bool isSuccessful = false;
    inputStream.Read(isSuccessful);
    if (isSuccessful) {
        inputStream.Read(clientComponent.m_playerID);
        assert(clientComponent.m_playerID < INVALID_PLAYER_ID);

        Event newEvent;
        newEvent.eventType = EventType::PLAYER_ADDED;
        newEvent.networking.playerID = clientComponent.m_playerID;
        PushEvent(newEvent);

        ILOG("Player %s %u has joined the game", clientComponent.m_name.c_str(), clientComponent.m_playerID);
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveStatePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream) const
{
    const bool isConnected = clientComponent.IsConnected();
    const bool isValid = clientComponent.m_deliveryManager.ReadState(inputStream);
    if (!isConnected || !isValid) {
        ILOG("State packet received but skipped");
        return;
    }

    ILOG("State packet received");

    bool isLastMoveTimestampDirty = false;
    inputStream.Read(isLastMoveTimestampDirty);
    if (isLastMoveTimestampDirty) {
        F32 lastMoveTimestamp = 0.0f;
        inputStream.Read(lastMoveTimestamp);
        clientComponent.m_RTT = Time::GetInstance().GetStartFrameTime() - lastMoveTimestamp;
        clientComponent.m_moves.RemoveMoves(lastMoveTimestamp);
    }

    clientComponent.m_replicationManager.Read(inputStream);
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

    Event newEvent;
    newEvent.eventType = EventType::PLAYER_REMOVED;
    newEvent.networking.entity = clientComponent.m_entity;
    PushEvent(newEvent);

    // TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    clientComponent.m_entity = INVALID_ENTITY;
    // Clear linkingContext
    // Clear networkGameObjects
}
}
