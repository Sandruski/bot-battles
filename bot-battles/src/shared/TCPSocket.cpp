#include "TCPSocket.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::shared_ptr<TCPSocket> TCPSocket::CreateIPv4()
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        NETLOG("socket");
        return nullptr;
    }

    return std::make_shared<TCPSocket>(sock, SocketAddress());
}

//----------------------------------------------------------------------------------------------------
TCPSocket::TCPSocket()
    : m_socket()
    , m_remoteSocketAddress()
{
}

//----------------------------------------------------------------------------------------------------
TCPSocket::TCPSocket(const SOCKET& socket, const SocketAddress& socketAddress)
    : m_socket(socket)
    , m_remoteSocketAddress(socketAddress)
{
}

//----------------------------------------------------------------------------------------------------
TCPSocket::~TCPSocket()
{
    shutdown(m_socket, SD_BOTH);
    closesocket(m_socket);
}

//----------------------------------------------------------------------------------------------------
bool TCPSocket::SetNoDelay(bool isNoDelay)
{
    int iResult = setsockopt(m_socket, SOL_SOCKET, TCP_NODELAY, reinterpret_cast<const char*>(&isNoDelay), sizeof(isNoDelay));
    if (iResult == SOCKET_ERROR) {
        NETLOG("setsockopt");
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool TCPSocket::SetNonBlockingMode(bool isNonBlockingMode)
{
    int iResult = ioctlsocket(m_socket, FIONBIO, reinterpret_cast<u_long*>(&isNonBlockingMode));
    if (iResult == SOCKET_ERROR) {
        NETLOG("ioctlsocket");
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool TCPSocket::SetReuseAddress(bool isReuseAddress)
{
    int iResult = setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&isReuseAddress), sizeof(isReuseAddress));
    if (iResult == SOCKET_ERROR) {
        NETLOG("setsockopt");
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool TCPSocket::Bind(const SocketAddress& socketAddress)
{
    int iResult = bind(m_socket, &socketAddress.m_sockAddr, sizeof(socketAddress.m_sockAddr));
    if (iResult == SOCKET_ERROR) {
        NETLOG("bind");
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool TCPSocket::Listen(U32 backlog)
{
    int iResult = listen(m_socket, backlog);
    if (iResult == SOCKET_ERROR) {
        NETLOG("listen");
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool TCPSocket::Connect(const SocketAddress& socketAddress)
{
    int iResult = connect(m_socket, &socketAddress.m_sockAddr, sizeof(socketAddress.m_sockAddr));
    if (iResult == SOCKET_ERROR) {
        int iError = WSAGetLastError();
        if (iError != WSAEWOULDBLOCK) {
            NETLOG("connect");
            return false;
        }

        fd_set writeSet;
        FD_ZERO(&writeSet);
        FD_SET(m_socket, &writeSet);

        timeval timeout;
        timeout.tv_sec = 1; // TODO: MACRO
        timeout.tv_usec = 0;
        iResult = select(0, nullptr, &writeSet, nullptr, &timeout);
        if (iResult == 0 || iResult == SOCKET_ERROR) {
            NETLOG("select");
            return false;
        }

        m_remoteSocketAddress = socketAddress;

        return true;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
std::shared_ptr<TCPSocket> TCPSocket::Accept(SocketAddress& socketAddress)
{
    socklen_t sockAddrLength = sizeof(socketAddress.m_sockAddr);
    SOCKET sock = accept(m_socket, &socketAddress.m_sockAddr, &sockAddrLength);
    if (sock == INVALID_SOCKET) {
        NETLOG("accept");
        return nullptr;
    }

    return std::make_shared<TCPSocket>(sock, socketAddress);
}

//----------------------------------------------------------------------------------------------------
I32 TCPSocket::Send(const void* data, int length)
{
    int iResult = send(m_socket, static_cast<const char*>(data), length, 0);
    if (iResult == SOCKET_ERROR) {
        NETLOG("send");
        int iError = WSAGetLastError();
        return -iError;
    }

    return iResult;
}

//----------------------------------------------------------------------------------------------------
I32 TCPSocket::Receive(void* buffer, int length)
{
    int iResult = recv(m_socket, static_cast<char*>(buffer), length, 0);
    if (iResult == SOCKET_ERROR) {
        NETLOG("recv");
        int iError = WSAGetLastError();
        return -iError;
    }

    return iResult;
}

//----------------------------------------------------------------------------------------------------
const SOCKET& TCPSocket::GetSocket() const
{
    return m_socket;
}

//----------------------------------------------------------------------------------------------------
SocketAddress TCPSocket::GetLocalSocketAddress() const
{
    sockaddr_in sockAddrIn;
    sockaddr* sockAddr = reinterpret_cast<sockaddr*>(&sockAddrIn);
    socklen_t sockAddrInLength = sizeof(sockAddrIn);
    int iResult = getsockname(m_socket, sockAddr, &sockAddrInLength);
    if (iResult == SOCKET_ERROR) {
        NETLOG("getsockname");
        return SocketAddress();
    }

    return SocketAddress(*sockAddr);
}

//----------------------------------------------------------------------------------------------------
SocketAddress TCPSocket::GetRemoteSocketAddress() const
{
    /*
    sockaddr_in sockAddrIn;
    sockaddr* sockAddr = reinterpret_cast<sockaddr*>(&sockAddrIn);
    socklen_t sockAddrInLength = sizeof(sockAddrIn);
    int iResult = getpeername(m_socket, sockAddr, &sockAddrInLength);
    if (iResult == SOCKET_ERROR) {
        NETLOG("getpeername");
        return SocketAddress();
    }

    return SocketAddress(*sockAddr);*/
    /*
    sockaddr_in sockAddrIn;
    sockaddr* sockAddr = reinterpret_cast<sockaddr*>(&sockAddrIn);
    socklen_t sockAddrInLength = sizeof(sockAddrIn);
    int iResult = getpeername(m_socket, sockAddr, &sockAddrInLength);
    if (iResult == SOCKET_ERROR) {
        NETLOG("getpeername");
    }*/

    return m_remoteSocketAddress;
}
}
