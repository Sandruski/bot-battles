#include "UDPSocket.h"

#include "SocketAddress.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::shared_ptr<UDPSocket> UDPSocket::CreateIPv4()
{
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        NETLOG("socket");
        return nullptr;
    }

    return std::make_shared<UDPSocket>(sock);
}

//----------------------------------------------------------------------------------------------------
UDPSocket::UDPSocket()
    : m_socket()
{
}

//----------------------------------------------------------------------------------------------------
UDPSocket::UDPSocket(const SOCKET& socket)
    : m_socket(socket)
{
}

//----------------------------------------------------------------------------------------------------
UDPSocket::~UDPSocket()
{
    shutdown(m_socket, SD_BOTH);
    closesocket(m_socket);
}

//----------------------------------------------------------------------------------------------------
bool UDPSocket::SetNonBlockingMode(bool isNonBlockingMode)
{
    int iResult = ioctlsocket(m_socket, FIONBIO, reinterpret_cast<u_long*>(&isNonBlockingMode));
    if (iResult == SOCKET_ERROR) {
        NETLOG("ioctlsocket");
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool UDPSocket::SetReuseAddress(bool isReuseAddress)
{
    int iResult = setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&isReuseAddress), sizeof(int));
    if (iResult == SOCKET_ERROR) {
        NETLOG("setsockopt");
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool UDPSocket::Bind(const SocketAddress& socketAddress)
{
    int iResult = bind(m_socket, &socketAddress.m_sockAddr, sizeof(socketAddress.m_sockAddr));
    if (iResult == SOCKET_ERROR) {
        NETLOG("bind");
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
I32 UDPSocket::SendTo(const void* data, int length, const SocketAddress& toSocketAddress)
{
    int iResult = sendto(m_socket, static_cast<const char*>(data), length, 0, &toSocketAddress.m_sockAddr, sizeof(toSocketAddress.m_sockAddr));
    if (iResult == SOCKET_ERROR) {
        NETLOG("sendto");
        int iError = WSAGetLastError();
        return -iError;
    }

    return iResult;
}

//----------------------------------------------------------------------------------------------------
I32 UDPSocket::ReceiveFrom(void* buffer, int length, SocketAddress& fromSocketAddress)
{
    socklen_t fromSockAddrLength = sizeof(fromSocketAddress.m_sockAddr);
    int iResult = recvfrom(m_socket, static_cast<char*>(buffer), length, 0, &fromSocketAddress.m_sockAddr, &fromSockAddrLength);
    if (iResult == SOCKET_ERROR) {
        int iError = WSAGetLastError();
        if (iError != WSAEWOULDBLOCK) {
            NETLOG("recvfrom");
        }
        return -iError;
    }

    return iResult;
}
}
