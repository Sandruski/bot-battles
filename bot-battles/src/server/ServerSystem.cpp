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
			LogError("WSAStartup");
			return false;
		}

		std::shared_ptr<SingletonServerComponent> server = g_game->GetSingletonServerComponent();

		int addressFamily = AF_INET; // IPv4
		int type = SOCK_DGRAM; // UDP
		int protocol = IPPROTO_UDP;
		server->m_socket = socket(addressFamily, type, protocol);
		if (server->m_socket == INVALID_SOCKET)
		{
			LogError("socket");
			return false;
		}

		int enable = 1;
		iResult = setsockopt(server->m_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&enable), sizeof(enable));
		if (iResult == SOCKET_ERROR)
		{
			LogError("setsockopt");
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
			LogError("bind");
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ServerSystem::Update(F32 dt)
	{
		std::shared_ptr<SingletonServerComponent> server = g_game->GetSingletonServerComponent();

		char buffer[1024];

		int flags = 0;
		SOCKADDR_IN fromAddress;
		int fromSize = sizeof(fromAddress);
		int iResult = recvfrom(server->m_socket, buffer, 1024, flags, reinterpret_cast<SOCKADDR*>(&fromAddress), &fromSize);
		if (iResult == SOCKET_ERROR)
		{
			LogError("recvfrom");
			return false;
		}

		LOG("Packet received from address %s port %u", fromAddress.sin_addr, ntohs(fromAddress.sin_port));

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ServerSystem::ShutDown()
	{
		// WSACleanup() omitted because Windows will clean up Winsock for us anyway

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	void ServerSystem::LogError(const char* message) const
	{
		wchar_t* s = nullptr;
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, 
			WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPWSTR>(&s), 
			0, 
			NULL);

		LOG("%s: %s", message, s);

		LocalFree(s);
	}
}