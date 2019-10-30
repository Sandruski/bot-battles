#include "ClientSystem.h"

#include "Game.h"
#include "SingletonClientComponent.h"
#include "SocketAddress.h"
#include "UDPSocket.h"

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
			NETLOG("WSAStartup");
			return false;
		}

		std::shared_ptr<SingletonClientComponent> client = g_game->GetSingletonClientComponent();
		client->m_socketAddress = SocketAddress::CreateIPv4("127.0.0.1", "9999");
		client->m_socket = UDPSocket::CreateIPv4();

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ClientSystem::Update()
	{
		std::shared_ptr<SingletonClientComponent> client = g_game->GetSingletonClientComponent();
		switch (client->m_state)
		{
		case SingletonClientComponent::ClientState::SAYING_HELLO:
		{
			UpdateSayingHelloPacket();
			break;
		}

		case SingletonClientComponent::ClientState::WELCOMED:
		{
			UpdateSendingInputPacket();
			break;
		}

		default:
		{
			break;
		}
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool ClientSystem::ShutDown()
	{
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	void ClientSystem::UpdateSayingHelloPacket()
	{
	}

	//----------------------------------------------------------------------------------------------------
	void ClientSystem::UpdateSendingInputPacket()
	{
	}
}
