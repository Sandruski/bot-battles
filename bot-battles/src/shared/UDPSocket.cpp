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

    return std::make_shared<UDPSocket>(sock, false);
}

//----------------------------------------------------------------------------------------------------
UDPSocket::UDPSocket()
    : m_socket()
    , m_isNonBlockingMode(false)
{
}

//----------------------------------------------------------------------------------------------------
UDPSocket::UDPSocket(SOCKET socket, bool isNonBlockingMode)
    : m_socket(socket)
    , m_isNonBlockingMode(isNonBlockingMode)
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
    u_long arg = isNonBlockingMode;
    int iResult = ioctlsocket(m_socket, FIONBIO, &arg);
    if (iResult == SOCKET_ERROR) {
        NETLOG("ioctlsocket");
        return false;
    }

    m_isNonBlockingMode = isNonBlockingMode;

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
bool UDPSocket::SendTo(const void* data, int length, const SocketAddress& toSocketAddress)
{
    int iResult = sendto(m_socket, static_cast<const char*>(data), length, 0, &toSocketAddress.m_sockAddr, sizeof(toSocketAddress.m_sockAddr));
    if (iResult == SOCKET_ERROR) {
        NETLOG("sendto");
        return false;
    }

    return true;
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
