#ifndef __CLIENT_PROXY_H__
#define __CLIENT_PROXY_H__

#include "Move.h"
#include "ServerReplicationManager.h"
#include "SocketAddress.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class ClientProxy {
public:
    ClientProxy();
    ClientProxy(const SocketAddress& socketAddress, const char* name);
    ~ClientProxy();

    void UpdateLastPacketTime();

    const SocketAddress& GetSocketAddress() const
    {
        return m_socketAddress;
    }

    const char* GetName() const
    {
        return m_name.c_str();
    }

    ServerReplicationManager& GetReplicationManager()
    {
        return m_replicationManager;
    }

public:
    Entity m_entity;

    bool m_isLastMoveTimestampDirty;

private:
    ServerReplicationManager m_replicationManager;

    SocketAddress m_socketAddress;
    std::string m_name;

    std::deque<Move> m_unprocessedMoves;

    F32 m_lastPacketTime;
};
}

#endif
