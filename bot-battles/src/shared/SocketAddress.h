#ifndef __SOCKET_ADDRESS_H__
#define __SOCKET_ADDRESS_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class SocketAddress {
    friend class UDPSocket;

public:
    static std::shared_ptr<SocketAddress> CreateIPv4(U32 address, U16 port);
    static std::shared_ptr<SocketAddress> CreateIPv4(const char* address, const char* port);

public:
    SocketAddress();
    SocketAddress(U32 address, U16 port);
    SocketAddress(const sockaddr& sockaddr);
    ~SocketAddress();

private:
    sockaddr m_sockAddr;
};
}

#endif
