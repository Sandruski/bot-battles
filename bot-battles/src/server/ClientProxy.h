#ifndef __CLIENT_PROXY_H__
#define __CLIENT_PROXY_H__

#include "DeliveryManagerServer.h"
#include "Input.h"
#include "SocketAddress.h"

namespace sand {

class ReplicationManagerServer;

//----------------------------------------------------------------------------------------------------
class ClientProxy {
public:
    ClientProxy(const SocketAddress& socketAddress, const char* name);

    const SocketAddress& GetSocketAddress() const;
    const char* GetName() const;

    void UpdateLastPacketTime();
    F32 GetLastPacketTime() const;

public:
    std::shared_ptr<ReplicationManagerServer> m_replicationManager;
    DeliveryManagerServer m_deliveryManager;

    CircularBuffer<Input, MAX_FRAMES> m_inputBuffer;

    U32 m_lastAckdFrame;
    F32 m_timestamp;

private:
    SocketAddress m_socketAddress;
    std::string m_name;

    F32 m_lastPacketTime;
};
}

#endif
