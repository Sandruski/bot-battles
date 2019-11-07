#include "SocketAddress.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::shared_ptr<SocketAddress> SocketAddress::CreateIPv4(U32 address, U16 port)
{
    return std::make_shared<SocketAddress>(address, port);
}

//----------------------------------------------------------------------------------------------------
std::shared_ptr<SocketAddress> SocketAddress::CreateIPv4(const char* address, const char* port)
{
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    addrinfo* result;
    int iResult = getaddrinfo(address, port, &hints, &result);
    if (iResult == SOCKET_ERROR) {
        NETLOG("getaddrinfo");

        if (result != nullptr) {
            freeaddrinfo(result);
        }

        return nullptr;
    }

    while (result->ai_addr == nullptr && result->ai_next) {
        result = result->ai_next;
    }

    if (result->ai_addr == nullptr) {
        freeaddrinfo(result);

        return nullptr;
    }

    std::shared_ptr<SocketAddress> ret = std::make_shared<SocketAddress>(*result->ai_addr);

    freeaddrinfo(result);

    return ret;
}

//----------------------------------------------------------------------------------------------------
SocketAddress::SocketAddress()
    : m_sockAddr()
    , m_name()
{
}

//----------------------------------------------------------------------------------------------------
SocketAddress::SocketAddress(U32 address, U16 port)
    : m_sockAddr()
    , m_name()
{
    sockaddr_in* sockAddrIn = reinterpret_cast<sockaddr_in*>(&m_sockAddr);
    sockAddrIn->sin_family = AF_INET;
    sockAddrIn->sin_addr.S_un.S_addr = htonl(address);
    sockAddrIn->sin_port = htons(port);

    CreateName();
}

//----------------------------------------------------------------------------------------------------
SocketAddress::SocketAddress(const sockaddr& sockaddr)
    : m_sockAddr()
    , m_name()
{
    memcpy_s(&m_sockAddr, sizeof(m_sockAddr), &sockaddr, sizeof(sockaddr));

    CreateName();
}

//----------------------------------------------------------------------------------------------------
SocketAddress::~SocketAddress()
{
}

//----------------------------------------------------------------------------------------------------
const sockaddr_in* SocketAddress::GetSockAddrIn() const
{
    return reinterpret_cast<const sockaddr_in*>(&m_sockAddr);
}

//----------------------------------------------------------------------------------------------------
const char* SocketAddress::GetName() const
{
    return m_name.c_str();
}

//----------------------------------------------------------------------------------------------------
void SocketAddress::CreateName()
{
    sockaddr_in* sockAddrIn = reinterpret_cast<sockaddr_in*>(&m_sockAddr);
    char addr[INET_ADDRSTRLEN];
    inet_ntop(sockAddrIn->sin_family, &sockAddrIn->sin_addr, addr, INET_ADDRSTRLEN);
    U16 port = htons(sockAddrIn->sin_port);

    m_name += addr;
    m_name += ":";
    m_name += std::to_string(port);
}
}
