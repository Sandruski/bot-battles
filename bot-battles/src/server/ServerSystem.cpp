#include "ServerSystem.h"

#include "Game.h"
#include "SingletonServerComponent.h"

#include "Log.h"

#include <WinSock2.h>

namespace sand
{

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
		if (iResult == SOCKET_ERROR)
		{
			NETLOG("WSAStartup");
			return false;
		}

		std::shared_ptr<SingletonServerComponent> server = g_game->GetSingletonServerComponent();

		int addressFamily = AF_INET; // IPv4
		int type = SOCK_DGRAM; // UDP
		int protocol = IPPROTO_UDP;
		server->m_socket = socket(addressFamily, type, protocol);
		if (server->m_socket == INVALID_SOCKET)
		{
			NETLOG("socket");
			return false;
		}

		int enable = 1;
		iResult = setsockopt(server->m_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&enable), sizeof(enable));
		if (iResult == SOCKET_ERROR)
		{
			NETLOG("setsockopt");
			return false;
		}
		
		// Source IP address
		SOCKADDR_IN localAddress;
		localAddress.sin_family = AF_INET; // IPv4
		localAddress.sin_port = htons(server->m_port); // Port
		localAddress.sin_addr.S_un.S_addr = INADDR_ANY; // any local IP address

		iResult = bind(server->m_socket, reinterpret_cast<SOCKADDR*>(&localAddress), sizeof(localAddress));
		if (iResult == SOCKET_ERROR)
		{
			NETLOG("bind");
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ServerSystem::Update(F32 /*dt*/)
	{
		std::shared_ptr<SingletonServerComponent> server = g_game->GetSingletonServerComponent();

		char buffer[1024];

		int flags = 0;
		SOCKADDR_IN fromAddress;
		int fromSize = sizeof(fromAddress);
		int iResult = recvfrom(server->m_socket, buffer, 1024, flags, reinterpret_cast<SOCKADDR*>(&fromAddress), &fromSize);
		if (iResult == SOCKET_ERROR)
		{
			NETLOG("recvfrom");
			return false;
		}

		ILOG("Packet received from address %s port %u", fromAddress.sin_addr, ntohs(fromAddress.sin_port));

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ServerSystem::ShutDown()
	{
		// WSACleanup() omitted because Windows will clean up Winsock for us anyway

		return true;
	}
}
