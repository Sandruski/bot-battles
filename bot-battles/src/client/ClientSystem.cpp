#include "ClientSystem.h"

#include "Game.h"
#include "SingletonClientComponent.h"
#include "SocketAddress.h"
#include "UDPSocket.h"

#include "NetworkLog.h"

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
		WORD winsockVersion = MAKEWORD(2, 2);
		WSADATA winsockData;
		int iResult = WSAStartup(winsockVersion, &winsockData);
		if (iResult == SOCKET_ERROR)
		{
			NETWORK_LOG("WSAStartup");
			return false;
		}

		std::shared_ptr<SingletonClientComponent> client = g_game->GetSingletonClientComponent();
		client->m_socketAddress = SocketAddress::CreateIPv4("127.0.0.1", "9999");
		client->m_socket = UDPSocket::CreateIPv4();

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ClientSystem::Update(F32 /*dt*/)
	{
		std::shared_ptr<SingletonClientComponent> client = g_game->GetSingletonClientComponent();

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ClientSystem::ShutDown()
	{
		return true;
	}
}
