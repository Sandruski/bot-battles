#include "ServerSystem.h"

#include "Game.h"
#include "MemoryStream.h"
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
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::ShutDown()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendWelcomePacket(const SingletonServerComponent& /*server*/, const ClientProxy& /*clientProxy*/) const
{
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceivePacket(SingletonServerComponent& server, InputMemoryStream& stream, const SocketAddress& fromAddress) const
{
    auto it = server.m_addressToClientProxy.find(fromAddress);
    if (it == server.m_addressToClientProxy.end()) {
        ReceivePacketFromNewClient(server, stream, fromAddress);
    } else {
        ReceivePacketFromExistingClient(server, stream, fromAddress);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceivePacketFromNewClient(SingletonServerComponent& /*server*/, InputMemoryStream& stream, const SocketAddress& /*fromAddress*/) const
{
    PacketType type;
    stream.Read(type);
    if (type != PacketType::HELLO) {
        WLOG("Bad incoming packet from unknown client");
        return;
    }

    std::string name;
    stream.Read(name);
    // TODO: create a client proxy

    //SendWelcomePacket(server, )
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceivePacketFromExistingClient(SingletonServerComponent& /*server*/, InputMemoryStream& stream, const SocketAddress& /*fromAddress*/) const
{
    PacketType type;
    stream.Read(type);
    /*
    switch (type) {

    default: {
        break;
    }
    }
	*/
}
}
