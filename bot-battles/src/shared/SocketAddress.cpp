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
{
}

//----------------------------------------------------------------------------------------------------
SocketAddress::SocketAddress(U32 address, U16 port)
    : m_sockAddr()
{
    sockaddr_in* sockAddrIn = reinterpret_cast<sockaddr_in*>(&m_sockAddr);
    sockAddrIn->sin_family = AF_INET;
    sockAddrIn->sin_addr.S_un.S_addr = htonl(address);
    sockAddrIn->sin_port = htons(port);
}

//----------------------------------------------------------------------------------------------------
SocketAddress::SocketAddress(const sockaddr& sockaddr)
    : m_sockAddr()
{
    memcpy_s(&m_sockAddr, sizeof(m_sockAddr), &sockaddr, sizeof(sockaddr));
}

//----------------------------------------------------------------------------------------------------
SocketAddress::~SocketAddress()
{
}
}
