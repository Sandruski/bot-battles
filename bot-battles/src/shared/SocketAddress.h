#ifndef __SOCKET_ADDRESS_H__
#define __SOCKET_ADDRESS_H__

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class SocketAddress
	{
		friend class UDPSocket;

	public:
		static std::shared_ptr<SocketAddress> CreateIPv4(const char* address, const char* port);

	public:
		~SocketAddress();

	public:
		SocketAddress(const sockaddr& sockaddr);



	private:
		sockaddr m_sockAddr;
	};
}

#endif
