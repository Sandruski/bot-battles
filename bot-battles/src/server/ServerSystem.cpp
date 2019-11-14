#include "ServerSystem.h"

#include "ClientProxy.h"
#include "Game.h"
#include "LinkingContext.h"
#include "MemoryStream.h"
#include "MessageTypes.h"
#include "SingletonServerComponent.h"
#include "SocketAddress.h"
#include "UDPSocket.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ServerSystem::ServerSystem()
{
}

//----------------------------------------------------------------------------------------------------
ServerSystem::~ServerSystem()
{
}

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

    std::shared_ptr<SingletonServerComponent> server = g_game->GetSingletonServerComponent();
    server->m_socketAddress = SocketAddress::CreateIPv4(INADDR_ANY, static_cast<U16>(atoi("9999")));
    assert(server->m_socketAddress != nullptr);
    server->m_socket = UDPSocket::CreateIPv4();
    assert(server->m_socket != nullptr);
    server->m_socket->SetNonBlockingMode(true);
    server->m_socket->Bind(*server->m_socketAddress);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::Update()
{
    std::shared_ptr<SingletonServerComponent> server = g_game->GetSingletonServerComponent();

    ReceivePackets(*server);
    // TODO: ProcessQueuedPackets()

    return true;
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {
    case EventType::PLAYER_ADDED: {
        std::shared_ptr<SingletonServerComponent> server = g_game->GetSingletonServerComponent();
        std::shared_ptr<ClientProxy> clientProxy = server->GetClientProxy(event.server.playerID);
        const std::unordered_map<NetworkID, Entity>& networkIDToEntity = g_game->GetLinkingContext().GetNetworkIDToEntityMap();
        for (const auto& pair : networkIDToEntity) {
            clientProxy->GetReplicationManager().CreateEntityCommand(pair.first);
        }
        break;
    }
    case EventType::NET_ENTITY_ADDED: {
        std::shared_ptr<SingletonServerComponent> server = g_game->GetSingletonServerComponent();
        const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = server->GetPlayerIDToClientProxyMap();
        for (const auto& pair : playerIDToClientProxy) {
            pair.second->GetReplicationManager().CreateEntityCommand(event.netEntity.networkID);
        }
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendWelcomePacket(const SingletonServerComponent& server, PlayerID playerID, const SocketAddress& toSocketAddress) const
{
    OutputMemoryStream welcomePacket;
    welcomePacket.Write(ServerMessageType::WELCOME);
    const bool isSuccessful = playerID != INVALID_PLAYER_ID;
    welcomePacket.Write(isSuccessful);
    if (isSuccessful) {
        welcomePacket.Write(playerID);
    }

    ILOG("Sending welcome packet to player %u...", playerID);

    const bool result = SendPacket(server, welcomePacket, toSocketAddress);
    if (result) {
        ILOG("Welcome packet successfully sent to player %u", playerID);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendStatePacket(const SingletonServerComponent& /*server*/, PlayerID playerID, const SocketAddress& /*toSocketAddress*/) const
{
    OutputMemoryStream welcomePacket;
    welcomePacket.Write(ServerMessageType::STATE);
    welcomePacket.Write(playerID);

    ILOG("Sending state packet to player %u...", playerID);

    // TODO
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::SendPacket(const SingletonServerComponent& server, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const
{
    return server.m_socket->SendTo(outputStream.GetPtr(), outputStream.GetByteLength(), toSocketAddress);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceivePackets(SingletonServerComponent& server)
{
    InputMemoryStream packet;
    SocketAddress fromSocketAddress;

    U32 receivedPacketCount = 0;

    while (receivedPacketCount < MAX_PACKETS_PER_FRAME) {

        I32 readByteCount = server.m_socket->ReceiveFrom(packet.GetPtr(), packet.GetByteCapacity(), fromSocketAddress);
        if (readByteCount > 0) {

            packet.ResetHead();
            packet.SetCapacity(readByteCount);
            ReceivePacket(server, packet, fromSocketAddress);

            ++receivedPacketCount;
        } else {

            int error = WSAGetLastError();
            if (error == WSAECONNRESET) {
                OnConnectionReset(fromSocketAddress);
            }

            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceivePacket(SingletonServerComponent& server, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress)
{
    ClientMessageType type;
    inputStream.Read(type);
    PlayerID playerID = INVALID_PLAYER_ID;
    switch (type) {
    case ClientMessageType::HELLO: {
        ReceiveHelloPacket(server, inputStream, fromSocketAddress, playerID);
        break;
    }

    case ClientMessageType::INPUT: {

        ReceiveInputPacket(server, inputStream, fromSocketAddress, playerID);
        break;
    }

    default: {
        WLOG("Unknown packet received from socket address %s", fromSocketAddress.GetName());
        break;
    }
    }
    assert(playerID != INVALID_PLAYER_ID);

    std::shared_ptr<ClientProxy> clientProxy = server.GetClientProxy(playerID);
    clientProxy->UpdateLastPacketTime();
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveHelloPacket(SingletonServerComponent& server, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress, PlayerID& playerID)
{
    playerID = server.GetPlayerID(fromSocketAddress);
    if (playerID == INVALID_PLAYER_ID) {
        std::string name;
        inputStream.Read(name);
        ILOG("Hello packet received from new player %s", name.c_str());

        playerID = server.AddPlayer(fromSocketAddress, name.c_str());
        if (playerID != INVALID_PLAYER_ID) {

            Event newEvent;
            newEvent.eventType = EventType::PLAYER_ADDED;
            newEvent.server.playerID = playerID;
            PushEvent(newEvent);

            ILOG("New player %s %u has joined the game", name.c_str(), playerID);
        }
    } else {
        std::shared_ptr<ClientProxy> clientProxy = server.GetClientProxy(playerID);
        ILOG("Hello packet received from existing player %s", clientProxy->GetName());
    }

    SendWelcomePacket(server, playerID, fromSocketAddress);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveInputPacket(SingletonServerComponent& server, InputMemoryStream& /*inputStream*/, const SocketAddress& /*fromSocketAddress*/, PlayerID playerID) const
{
    std::shared_ptr<ClientProxy> clientProxy = server.GetClientProxy(playerID);
    if (clientProxy == nullptr) {
        return;
    }

    // TODO: handle moves, etc.
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnConnectionReset(const SocketAddress& /*fromSocketAddress*/) const
{
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnDisconnect() const
{
}
}
