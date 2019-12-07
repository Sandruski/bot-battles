#include "ClientSystem.h"

#include "DeliveryManagerClient.h"
#include "GameClient.h"
#include "LinkingContext.h"
#include "MemoryStream.h"
#include "MessageTypes.h"
#include "Move.h"
#include "ReceivedPacket.h"
#include "ReplicationManagerClient.h"
#include "SingletonClientComponent.h"
#include "SingletonInputComponent.h"
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

    std::shared_ptr<SingletonClientComponent> singletonClient = g_gameClient->GetSingletonClientComponent();
    singletonClient->m_socketAddress = SocketAddress::CreateIPv4("127.0.0.1", "9999");
    assert(singletonClient->m_socketAddress != nullptr);
    singletonClient->m_socket = UDPSocket::CreateIPv4();
    assert(singletonClient->m_socket != nullptr);
    singletonClient->m_socket->SetNonBlockingMode(true);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::Update()
{
    std::shared_ptr<SingletonClientComponent> singletonClient = g_gameClient->GetSingletonClientComponent();

    EnqueueIncomingPackets(*singletonClient);
    ProcessIncomingPackets(*singletonClient);

    SendOutgoingPackets(*singletonClient);

    return true;
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::SendOutgoingPackets(SingletonClientComponent& singletonClient) const
{
    const bool isConnected = singletonClient.IsConnected();
    if (!isConnected) {
        UpdateSendHelloPacket(singletonClient);
    } else {
        UpdateSendInputPacket(singletonClient);
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::UpdateSendHelloPacket(SingletonClientComponent& singletonClient) const
{
    float time = Time::GetInstance().GetTime();
    float nextHelloTime = singletonClient.GetNextHelloTime();
    if (time >= nextHelloTime) {
        const bool result = SendHelloPacket(singletonClient);
        if (result) {
            singletonClient.m_lastTime = time;
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::UpdateSendInputPacket(SingletonClientComponent& singletonClient) const
{
    float time = Time::GetInstance().GetTime();
    float nextInputTime = singletonClient.GetNextInputTime();
    if (time >= nextInputTime) {
        std::shared_ptr<SingletonInputComponent> singletonInput = g_gameClient->GetSingletonInputComponent();
        const bool result = SendInputPacket(singletonClient, *singletonInput);
        if (result) {
            singletonClient.m_lastTime = time;
            singletonInput->ClearMoves();
        }
    }
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendHelloPacket(const SingletonClientComponent& singletonClient) const
{
    OutputMemoryStream helloPacket;
    helloPacket.Write(ClientMessageType::HELLO);
    helloPacket.Write(singletonClient.m_name);

    ILOG("Sending hello packet to server...");

    const bool result = SendPacket(singletonClient, helloPacket);
    if (result) {
        ILOG("Hello packet successfully sent to server");
    }

    return result;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendInputPacket(const SingletonClientComponent& singletonClient, const SingletonInputComponent& singletonInput) const
{
    if (!singletonInput.HasMoves()) {
        return false;
    }

    OutputMemoryStream inputPacket;
    inputPacket.Write(ClientMessageType::INPUT);
    inputPacket.Write(singletonClient.m_playerID);
    g_gameClient->GetDeliveryManager().WriteState(inputPacket);

    U32 totalMoveCount = singletonInput.GetMoveCount();
    U32 startIndex = totalMoveCount > MAX_MOVES_PER_PACKET ? totalMoveCount - MAX_MOVES_PER_PACKET : 0;
    U32 moveCount = totalMoveCount - startIndex;
    inputPacket.Write(moveCount, GetRequiredBits<MAX_MOVES_PER_PACKET>::value);
    for (U32 i = startIndex; i < totalMoveCount; ++i) {
        const Move& move = singletonInput.GetMove(i);
        move.Write(inputPacket);
    }

    ILOG("Sending input packet to server...");

    const bool result = SendPacket(singletonClient, inputPacket);
    if (result) {
        ILOG("Input packet successfully sent to server");
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendPacket(const SingletonClientComponent& singletonClient, const OutputMemoryStream& outputStream) const
{
    return singletonClient.m_socket->SendTo(outputStream.GetPtr(), outputStream.GetByteLength(), *singletonClient.m_socketAddress);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::EnqueueIncomingPackets(SingletonClientComponent& singletonClient) const
{
    InputMemoryStream packet;
    SocketAddress fromSocketAddress;

    U32 receivedPacketCount = 0;

    F32 time = Time::GetInstance().GetTime();

    while (receivedPacketCount < MAX_PACKETS_PER_FRAME) {
        I32 readByteCount = singletonClient.m_socket->ReceiveFrom(packet.GetPtr(), packet.GetByteCapacity(), fromSocketAddress);
        if (readByteCount > 0) {
            packet.SetCapacity(readByteCount);
            packet.ResetHead();

#ifdef _SIMULATE_REAL_WORLD_CONDITIONS
            F32 randomDropPacketChance = GetRandomFloat(0.0f, 1.0f);
            if (randomDropPacketChance >= DROP_PACKET_CHANCE) {
                F32 randomJitterFactor = GetRandomFloat(-1.0f, 1.0f);
                F32 simulatedTime = time + SIMULATED_LATENCY + SIMULATED_JITTER * randomJitterFactor;

                auto it = singletonClient.m_receivedPackets.end();
                while (it != singletonClient.m_receivedPackets.begin()) {
                    --it;
                    F32 timestamp = it->GetTimestamp();
                    if (timestamp < simulatedTime) {
                        ++it;
                        break;
                    }
                }
                singletonClient.m_receivedPackets.emplace_back(packet, fromSocketAddress, simulatedTime);
            }
#else
            singletonClient.m_receivedPackets.emplace_back(packet, fromSocketAddress, time);
#endif

            ++receivedPacketCount;
        } else if (readByteCount == -WSAECONNRESET) {
            OnConnectionReset(singletonClient);
        } else if (readByteCount == 0 || -WSAEWOULDBLOCK) {
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ProcessIncomingPackets(SingletonClientComponent& singletonClient) const
{
    F32 time = Time::GetInstance().GetTime();

    while (!singletonClient.m_receivedPackets.empty()) {
        ReceivedPacket& receivedPacket = singletonClient.m_receivedPackets.front();
        F32 timestamp = receivedPacket.GetTimestamp();
        if (time >= timestamp) {
            ReceivePacket(singletonClient, receivedPacket.GetBuffer());
            singletonClient.m_receivedPackets.pop_front();
        } else {
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceivePacket(SingletonClientComponent& singletonClient, InputMemoryStream& inputStream) const
{
    ServerMessageType type;
    inputStream.Read(type);
    switch (type) {
    case ServerMessageType::WELCOME: {
        ReceiveWelcomePacket(singletonClient, inputStream);
        break;
    }

    case ServerMessageType::STATE: {
        ReceiveStatePacket(singletonClient, inputStream);
        break;
    }

    default: {
        WLOG("Unknown packet received");
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveWelcomePacket(SingletonClientComponent& singletonClient, InputMemoryStream& inputStream) const
{
    const bool isConnected = singletonClient.IsConnected();
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
            singletonClient.m_playerID = playerID;
            ILOG("Player %s %u has joined the game", singletonClient.m_name.c_str(), singletonClient.m_playerID);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveStatePacket(SingletonClientComponent& singletonClient, InputMemoryStream& inputStream) const
{
    const bool isConnected = singletonClient.IsConnected();
    const bool isValid = g_gameClient->GetDeliveryManager().ReadState(inputStream);
    if (!isConnected || !isValid) {
        ILOG("State packet received but skipped");
        return;
    }

    ILOG("State packet received");

    // TODO
    g_gameClient->GetReplicationManager().Read(inputStream);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::OnConnectionReset(SingletonClientComponent& singletonClient) const
{
    OnDisconnect(singletonClient);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::OnDisconnect(SingletonClientComponent& singletonClient) const
{
    singletonClient.m_playerID = INVALID_PLAYER_ID;
    // Clear linkingContext
    // Clear networkGameObjects
}
}
