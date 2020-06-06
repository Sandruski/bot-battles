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

    F32 GetRatio() const;

    void Reset();

public:
    std::shared_ptr<ReplicationManagerServer> m_replicationManager;
    DeliveryManagerServer m_deliveryManager;

    CircularBuffer<Input, MAX_INPUTS> m_inputBuffer;
    /*
    -Add: ServerSystem
    -Remove: OutputSystemServer
    */
    U32 m_lastAckdFrame;
    F32 m_lastPacketTime;
    F32 m_timestamp;

    U32 m_damageInflicted;
    U32 m_damageReceived;

private:
    SocketAddress m_socketAddress;
    std::string m_name;
};
}

#endif
