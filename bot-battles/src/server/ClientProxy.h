#ifndef __CLIENT_PROXY_H__
#define __CLIENT_PROXY_H__

#include "DeliveryManagerServer.h"
#include "Moves.h"
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

    Moves m_moves;

    bool m_isTimestampDirty;
    F32 m_timestamp;

    bool m_isFrameDirty;
    U32 m_frame;

private:
    SocketAddress m_socketAddress;
    std::string m_name;

    F32 m_lastPacketTime;
};
}

#endif
