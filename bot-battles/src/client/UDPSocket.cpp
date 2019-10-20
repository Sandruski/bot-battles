#include "UDPSocket.h"

#include "SocketAddress.h"
#include "NetworkLog.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	std::shared_ptr<UDPSocket> UDPSocket::CreateIPv4()
	{
		SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock == INVALID_SOCKET)
		{
			NETWORK_LOG("socket");
			return nullptr;
		}

		return std::make_shared<UDPSocket>(sock);
	}

	//----------------------------------------------------------------------------------------------------
	UDPSocket::~UDPSocket()
	{
		shutdown(m_socket, SD_BOTH);
		closesocket(m_socket);
	}
	
	//----------------------------------------------------------------------------------------------------
	bool UDPSocket::Bind(const SocketAddress& socketAddress)
	{
		int iResult = bind(m_socket, &socketAddress.m_sockAddr, sizeof(socketAddress.m_sockAddr));
		if (iResult == SOCKET_ERROR)
		{
			NETWORK_LOG("bind");
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool UDPSocket::SendTo(const void* data, int length, const SocketAddress& toSocketAddress)
	{
		int iResult = sendto(m_socket, static_cast<const char*>(data), length, 0, &toSocketAddress.m_sockAddr, sizeof(toSocketAddress.m_sockAddr));
		if (iResult == SOCKET_ERROR)
		{
			NETWORK_LOG("sendto");
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool UDPSocket::ReceiveFrom(void* buffer, int length, SocketAddress& fromSocketAddress)
	{
		int fromLength = sizeof(fromSocketAddress.m_sockAddr);
		int iResult = recvfrom(m_socket, static_cast<char*>(buffer), length, 0, &fromSocketAddress.m_sockAddr, &fromLength);
		if (iResult == SOCKET_ERROR)
		{
			NETWORK_LOG("recvfrom");
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	UDPSocket::UDPSocket(SOCKET socket)
		: m_socket(socket)
	{
	}
}
