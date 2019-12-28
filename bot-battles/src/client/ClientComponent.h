#ifndef __CLIENT_COMPONENT_H__
#define __CLIENT_COMPONENT_H__

#include "ReceivedPacket.h"

namespace sand {

class SocketAddress;
class UDPSocket;

//----------------------------------------------------------------------------------------------------
// System Component
struct ClientComponent {

    ClientComponent();

    bool IsConnected() const;

    F32 GetNextHelloTime() const;
    F32 GetNextInputTime() const;

    std::shared_ptr<UDPSocket> m_socket;
    std::shared_ptr<SocketAddress> m_socketAddress;

    std::deque<ReceivedPacket> m_receivedPackets;

    std::string m_name;
    PlayerID m_playerID;

    F32 m_helloTime;
    F32 m_inputTime;
    F32 m_lastTime;
};
}

#endif
