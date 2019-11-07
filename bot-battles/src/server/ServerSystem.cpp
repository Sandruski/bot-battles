#include "ServerSystem.h"

#include "ClientProxy.h"
#include "Game.h"
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
    server->m_socket = UDPSocket::CreateIPv4();
    server->m_socket->Bind(*server->m_socketAddress);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::Update()
{
    std::shared_ptr<SingletonServerComponent> server = g_game->GetSingletonServerComponent();

    // TODO

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::ShutDown()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendWelcomePacket(const SingletonServerComponent& server, PlayerID playerID, const SocketAddress& toSocketAddress) const
{
    OutputMemoryStream welcomePacket;
    welcomePacket.Write(ServerMessageType::WELCOME);
    welcomePacket.Write(playerID);

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

    // TODO
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::SendPacket(const SingletonServerComponent& server, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const
{
    server.m_socket->SendTo(outputStream.GetPtr(), outputStream.GetByteLength(), toSocketAddress);

    return true;
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceivePacket(SingletonServerComponent& server, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress) const
{
    ClientMessageType type;
    inputStream.Read(type);
    switch (type) {
    case ClientMessageType::HELLO: {
        ReceiveHelloPacket(server, inputStream, fromSocketAddress);
        break;
    }

    case ClientMessageType::INPUT: {
        ReceiveInputPacket(server, inputStream, fromSocketAddress);
        break;
    }

    default: {
        WLOG("Unknown packet type received from socket address %s", fromSocketAddress.GetName());
        break;
    }
    }

    std::shared_ptr<ClientProxy> clientProxy = server.GetClientProxy(fromSocketAddress);
    clientProxy->UpdateLastPacketTime();
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveHelloPacket(SingletonServerComponent& server, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress) const
{
    std::string name;
    inputStream.Read(name);

    const PlayerID playerID = server.AddPlayer(fromSocketAddress, name.c_str());
    if (playerID != INVALID_PLAYER_ID) {

        ILOG("Player %s with the ID %u has joined the game", name.c_str(), playerID);

        /* TODO (GAMEPLAY): add a new entity for this player. 
		The entity MUST be linked to the playerID somewhere so it can be removed properly if necessary.
        E.g. send an event, etc. */

        /* TODO: init the replication manager with everything we know about!
		Everything we know about is stored in a map here and contains all objects that clients have registered to the server.
		*/
    }

    SendWelcomePacket(server, playerID, fromSocketAddress);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveInputPacket(SingletonServerComponent& /*server*/, InputMemoryStream& /*inputStream*/, const SocketAddress& /*fromSocketAddress*/) const
{
    // Handle moves, etc.
}
}
