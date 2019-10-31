#include "ClientSystem.h"

#include "Game.h"
#include "SingletonClientComponent.h"
#include "SocketAddress.h"
#include "UDPSocket.h"
#include "MemoryStream.h"

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
		case SingletonClientComponent::ClientState::SAY_HELLO:
		{
			UpdateSayHello(*client);
			break;
		}

		case SingletonClientComponent::ClientState::SEND_INPUT:
		{
			UpdateSendInput(*client);
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
	void ClientSystem::UpdateSayHello(SingletonClientComponent& client) const
	{
		float time = Time::GetInstance().GetTime();
		float nextTime = client.m_lastTime + client.m_timeBetweenSayHello;
		if (time >= nextTime)
		{
			SendHelloPacket(client);

			client.m_lastTime = time;
		}
	}

	//----------------------------------------------------------------------------------------------------
	void ClientSystem::UpdateSendInput(SingletonClientComponent& client) const
	{
		float time = Time::GetInstance().GetTime();
		float nextTime = client.m_lastTime + client.m_timeBetweenSendInput;
		if (time >= nextTime)
		{
			SendInputPacket(client);

			client.m_lastTime = time;
		}
	}

	//----------------------------------------------------------------------------------------------------
	void ClientSystem::SendHelloPacket(const SingletonClientComponent& client) const
	{
		OutputMemoryStream helloPacket;
		helloPacket.Write(PacketType::HELLO);
		helloPacket.Write(client.m_name);

		SendPacket(client, helloPacket);
	}

	//----------------------------------------------------------------------------------------------------
	void ClientSystem::SendInputPacket(const SingletonClientComponent& /*client*/) const
	{
	}

	//----------------------------------------------------------------------------------------------------
	void ClientSystem::ReceiveWelcomePacket(SingletonClientComponent& client, InputMemoryStream& stream) const
	{
		if (client.m_state != SingletonClientComponent::ClientState::SAY_HELLO)
		{
			return;
		}

		stream.Read(client.m_playerID);
		ILOG("Player %s has been welcomed as %u", client.m_name.c_str(), client.m_playerID);

		client.m_state = SingletonClientComponent::ClientState::SEND_INPUT;
	}

	//----------------------------------------------------------------------------------------------------
	void ClientSystem::SendPacket(const SingletonClientComponent& client, const OutputMemoryStream& stream) const
	{
		client.m_socket->SendTo(stream.GetPtr(), stream.GetByteLength(), *client.m_socketAddress);
		// TODO: how do we handle the return of the SentTo method?
	}
}
