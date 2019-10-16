#include "ClientSystem.h"

#include "Game.h"
#include "SingletonClientComponent.h"
#include "SingletonServerComponent.h"

#include "Log.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

namespace sand
{

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
		/*
		WORD winsockVersion = MAKEWORD(2, 2);
		WSADATA winsockData;
		int iResult = WSAStartup(winsockVersion, &winsockData);
		if (iResult == SOCKET_ERROR)
		{
			LogError("WSAStartup");
			return false;
		}
		*/

		std::shared_ptr<SingletonClientComponent> client = g_game->GetSingletonClientComponent();

		int addressFamily = AF_INET; // IPv4
		int type = SOCK_DGRAM; // UDP
		int protocol = IPPROTO_UDP;
		client->m_socket = socket(addressFamily, type, protocol);
		if (client->m_socket == INVALID_SOCKET)
		{
			LogError("socket");
			return false;
		}

		std::shared_ptr<SingletonServerComponent> server = g_game->GetSingletonServerComponent();

		// Destination IP address
		client->toAddress.sin_family = AF_INET; // IPv4
		client->toAddress.sin_port = htons(client->m_port); // Port
		inet_pton(AF_INET, server->m_address, &client->toAddress.sin_addr);

		// bind() omitted because we will send data on a socket before receiving, so Windows will implicitly bind the socket for us to some unused port 

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ClientSystem::Update(F32 dt)
	{
		std::shared_ptr<SingletonClientComponent> client = g_game->GetSingletonClientComponent();

		char buffer[1024] = "Hello World!";

		int flags = 0;
		int iResult = sendto(client->m_socket, buffer, 1024, flags, reinterpret_cast<SOCKADDR*>(&client->toAddress), sizeof(client->toAddress));
		if (iResult == SOCKET_ERROR)
		{
			LogError("sendto");
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ClientSystem::ShutDown()
	{
		// WSACleanup() omitted because Windows will clean up Winsock for us anyway

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	void ClientSystem::LogError(const char* message) const
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