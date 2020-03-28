#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#include "SocketAddress.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class TCPSocket {
public:
    static std::shared_ptr<TCPSocket> CreateIPv4();

public:
    TCPSocket();
    TCPSocket(const SOCKET& socket);
    ~TCPSocket();

    bool SetNonBlockingMode(bool isNonBlockingMode);
    bool SetReuseAddress(bool isReuseAddress);
    bool Bind(const SocketAddress& socketAddress);
    bool Listen(U32 backlog);
    bool Connect(const SocketAddress& socketAddress);
    std::shared_ptr<TCPSocket> Accept(SocketAddress& socketAddress);
    I32 Send(const void* data, int length);
    I32 Receive(void* buffer, int length);

    const SOCKET& GetSocket() const;
    SocketAddress GetLocalSocketAddress() const;
    SocketAddress GetRemoteSocketAddress() const;

private:
    SOCKET m_socket;
};
}

#endif
