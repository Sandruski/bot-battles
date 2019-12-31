#include "ClientSystem.h"

#include "ClientComponent.h"
#include "DeliveryManagerClient.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "MessageTypes.h"
#include "Move.h"
#include "MoveComponent.h"
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
        UpdateSendHelloPacket(clientComponent);
    } else {
        UpdateSendInputPacket(clientComponent);
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::UpdateSendHelloPacket(ClientComponent& clientComponent) const
{
    F32 time = Time::GetInstance().GetTime();
    F32 nextHelloTime = clientComponent.GetNextHelloTime();
    if (time >= nextHelloTime) {
        const bool result = SendHelloPacket(clientComponent);
        if (result) {
            clientComponent.m_lastDeliveryTimestamp = time;
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::UpdateSendInputPacket(ClientComponent& clientComponent) const
{
    F32 time = Time::GetInstance().GetTime();
    F32 nextInputTime = clientComponent.GetNextInputTime();
    if (time >= nextInputTime) {
        MoveComponent& moveComponent = g_gameClient->GetMoveComponent();
        const bool result = SendInputPacket(clientComponent, moveComponent);
        if (result) {
            clientComponent.m_lastDeliveryTimestamp = time;
        }
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
bool ClientSystem::SendInputPacket(const ClientComponent& clientComponent, MoveComponent& moveComponent) const
{
    if (!moveComponent.HasMoves()) {
        return false;
    }

    OutputMemoryStream inputPacket;
    inputPacket.Write(ClientMessageType::INPUT);
    inputPacket.Write(clientComponent.m_playerID);
    g_gameClient->GetDeliveryManager().WriteState(inputPacket);

    U32 totalMoveCount = moveComponent.GetMoveCount();
    U32 startIndex = totalMoveCount > MAX_MOVES_PER_PACKET ? totalMoveCount - MAX_MOVES_PER_PACKET : 0;
    U32 moveCount = totalMoveCount - startIndex;
    inputPacket.Write(moveCount, GetRequiredBits<MAX_MOVES_PER_PACKET>::value);
    for (U32 i = startIndex; i < totalMoveCount; ++i) {
        const Move& move = moveComponent.GetMove(i);
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
void ClientSystem::ReceiveIncomingPackets(ClientComponent& clientComponent) const
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
            OnConnectionReset(clientComponent);
        } else if (readByteCount == 0 || -WSAEWOULDBLOCK) {
            // TODO: graceful disconnection if readByteCount == 0?
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceivePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream) const
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
void ClientSystem::ReceiveWelcomePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream) const
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

        PlayerID playerID = INVALID_PLAYER_ID;
        inputStream.Read(playerID);
        if (playerID != INVALID_PLAYER_ID) {
            clientComponent.m_playerID = playerID;
            ILOG("Player %s %u has joined the game", clientComponent.m_name.c_str(), clientComponent.m_playerID);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveStatePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream) const
{
    const bool isConnected = clientComponent.IsConnected();
    const bool isValid = g_gameClient->GetDeliveryManager().ReadState(inputStream);
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

        // TODO: update RTT

        MoveComponent& moveComponent = g_gameClient->GetMoveComponent();
        moveComponent.RemoveProcessedMoves(lastMoveTimestamp);
    }

    g_gameClient->GetReplicationManager().Read(inputStream);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::OnConnectionReset(ClientComponent& clientComponent) const
{
    OnDisconnect(clientComponent);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::OnDisconnect(ClientComponent& clientComponent) const
{
    clientComponent.m_playerID = INVALID_PLAYER_ID;
    // Clear linkingContext
    // Clear networkGameObjects
}
}
