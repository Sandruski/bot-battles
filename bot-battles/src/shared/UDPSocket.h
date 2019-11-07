#ifndef __UDP_SOCKET_H__
#define __UDP_SOCKET_H__

namespace sand {

class SocketAddress;

//----------------------------------------------------------------------------------------------------
class UDPSocket {
public:
    static std::shared_ptr<UDPSocket> CreateIPv4();

public:
    UDPSocket();
    UDPSocket(SOCKET socket);
    ~UDPSocket();

    bool Bind(const SocketAddress& socketAddress);
    bool SendTo(const void* data, int length, const SocketAddress& toSocketAddress);
    bool ReceiveFrom(void* buffer, int length, SocketAddress& fromSocketAddress);

private:
    SOCKET m_socket;
};
}

#endif
