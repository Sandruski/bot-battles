#include "ClientSystem.h"

#include "ClientReplicationManager.h"
#include "Game.h"
#include "LinkingContext.h"
#include "MemoryStream.h"
#include "MessageTypes.h"
#include "Move.h"
#include "SingletonClientComponent.h"
#include "SocketAddress.h"
#include "UDPSocket.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ClientSystem::ClientSystem()
{
}

//----------------------------------------------------------------------------------------------------
ClientSystem::~ClientSystem()
{
}

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

    std::shared_ptr<SingletonClientComponent> client = g_game->GetSingletonClientComponent();
    client->m_socketAddress = SocketAddress::CreateIPv4("127.0.0.1", "9999");
    assert(client->m_socketAddress != nullptr);
    client->m_socket = UDPSocket::CreateIPv4();
    assert(client->m_socket != nullptr);
    client->m_socket->SetNonBlockingMode(true);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::Update()
{
    std::shared_ptr<SingletonClientComponent> client = g_game->GetSingletonClientComponent();

    ReceiveIncomingPackets(*client);
    // TODO: ProcessQueuedPackets()
    SendOutgoingPackets(*client);

    return true;
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::SendOutgoingPackets(SingletonClientComponent& client) const
{
    const bool isConnected = client.IsConnected();
    if (!isConnected) {
        UpdateSendHelloPacket(client);
    } else {
        UpdateSendInputPacket(client);
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::UpdateSendHelloPacket(SingletonClientComponent& client) const
{
    float time = Time::GetInstance().GetTime();
    float nextHelloTime = client.GetNextHelloTime();
    if (time >= nextHelloTime) {
        const bool result = SendHelloPacket(client);
        if (result) {
            client.m_lastTime = time;
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::UpdateSendInputPacket(SingletonClientComponent& client) const
{
    float time = Time::GetInstance().GetTime();
    float nextInputTime = client.GetNextInputTime();
    if (time >= nextInputTime) {
        const bool result = SendInputPacket(client);
        if (result) {
            client.m_lastTime = time;
            client.ClearMoves();
        }
    }
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendHelloPacket(const SingletonClientComponent& client) const
{
    OutputMemoryStream helloPacket;
    helloPacket.Write(ClientMessageType::HELLO);

    helloPacket.Write(client.m_name);

    ILOG("Sending hello packet to server...");

    const bool result = SendPacket(client, helloPacket);
    if (result) {
        ILOG("Hello packet successfully sent to server");
    }

    return result;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendInputPacket(const SingletonClientComponent& client) const
{
    if (!client.HasMoves()) {
        return false;
    }

    OutputMemoryStream inputPacket;
    inputPacket.Write(ClientMessageType::INPUT);

    U32 totalMoveCount = client.GetMoveCount();
    U32 startIndex = totalMoveCount > MAX_MOVES_PER_PACKET ? totalMoveCount - MAX_MOVES_PER_PACKET - 1 : 0;
    U32 moveCount = totalMoveCount - startIndex;
    inputPacket.Write(moveCount, GetRequiredBits<MAX_MOVES_PER_PACKET>::value);
    for (U32 i = startIndex; i < moveCount; ++i) {
        const Move& move = client.GetMove(i);
        move.Write(inputPacket, static_cast<U16>(SingletonInputComponent::MemberType::ALL));
    }

    ILOG("Sending input packet to server...");

    const bool result = SendPacket(client, inputPacket);
    if (result) {
        ILOG("Input packet successfully sent to server");
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendPacket(const SingletonClientComponent& client, const OutputMemoryStream& outputStream) const
{
    return client.m_socket->SendTo(outputStream.GetPtr(), outputStream.GetByteLength(), *client.m_socketAddress);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveIncomingPackets(SingletonClientComponent& client) const
{
    U32 receivedPacketCount = 0;

    while (receivedPacketCount < MAX_PACKETS_PER_FRAME) {

        InputMemoryStream packet;
        SocketAddress fromSocketAddress;

        I32 readByteCount = client.m_socket->ReceiveFrom(packet.GetPtr(), packet.GetByteCapacity(), fromSocketAddress);
        if (readByteCount > 0) {

            packet.ResetHead();
            packet.SetCapacity(readByteCount);
            ReceivePacket(client, packet);

            ++receivedPacketCount;
        } else {

            int error = WSAGetLastError();
            if (error == WSAECONNRESET) {
                OnConnectionReset(client);
            }

            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceivePacket(SingletonClientComponent& client, InputMemoryStream& inputStream) const
{
    ServerMessageType type;
    inputStream.Read(type);
    switch (type) {
    case ServerMessageType::WELCOME: {
        ReceiveWelcomePacket(client, inputStream);
        break;
    }

    case ServerMessageType::STATE: {
        ReceiveStatePacket(client, inputStream);
        break;
    }

    default: {
        WLOG("Unknown packet received");
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveWelcomePacket(SingletonClientComponent& client, InputMemoryStream& inputStream) const
{
    ILOG("Welcome packet received");

    if (client.IsConnected()) {
        return;
    }

    bool isSuccessful = false;
    inputStream.Read(isSuccessful);
    if (isSuccessful) {
        PlayerID playerID = INVALID_PLAYER_ID;
        inputStream.Read(playerID);
        if (playerID != INVALID_PLAYER_ID) {
            client.m_playerID = playerID;
            ILOG("Player %s %u has joined the game", client.m_name.c_str(), client.m_playerID);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveStatePacket(SingletonClientComponent& client, InputMemoryStream& inputStream) const
{
    ILOG("State packet received");

    if (!client.IsConnected()) {
        return;
    }

    // TODO
    g_game->GetClientReplicationManager().Read(inputStream);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::OnConnectionReset(SingletonClientComponent& client) const
{
    OnDisconnect(client);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::OnDisconnect(SingletonClientComponent& client) const
{
    client.m_playerID = INVALID_PLAYER_ID;
    // Clear linkingContext
    // Clear networkGameObjects
}
}
