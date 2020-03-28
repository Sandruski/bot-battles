#ifndef __SOCKET_ADDRESS_H__
#define __SOCKET_ADDRESS_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class SocketAddress {
    friend class UDPSocket;
    friend class TCPSocket;

public:
    static std::shared_ptr<SocketAddress> CreateIPv4(U32 address, U16 port);
    static std::shared_ptr<SocketAddress> CreateIPv4(const char* address, const char* port);

public:
    SocketAddress();
    SocketAddress(U32 address, U16 port);
    SocketAddress(const sockaddr& sockaddr);

    bool operator==(const SocketAddress& other) const
    {
        const sockaddr_in* sockAddrIn = GetSockAddrIn();
        const sockaddr_in* otherSockAddrIn = other.GetSockAddrIn();

        return (sockAddrIn->sin_family == otherSockAddrIn->sin_family
            && sockAddrIn->sin_addr.S_un.S_addr == otherSockAddrIn->sin_addr.S_un.S_addr
            && sockAddrIn->sin_port == otherSockAddrIn->sin_port);
    }

    const sockaddr_in* GetSockAddrIn() const;
    const char* GetName() const;

private:
    void CreateName();

private:
    sockaddr m_sockAddr;
    std::string m_name;
};
}

#endif
