#ifndef __CLIENT_PROXY_H__
#define __CLIENT_PROXY_H__

#include "Move.h"
#include "SocketAddress.h"

namespace sand {

class ReplicationManagerServer;
class DeliveryManagerServer;

//----------------------------------------------------------------------------------------------------
class ClientProxy {
public:
    ClientProxy();
    ClientProxy(const SocketAddress& socketAddress, const char* name);
    ~ClientProxy();

    void UpdateLastPacketTime();

    const Move& AddMove(const Move& move);

    const SocketAddress& GetSocketAddress() const
    {
        return m_socketAddress;
    }

    const char* GetName() const
    {
        return m_name.c_str();
    }

    const std::deque<Move>& GetUnprocessedMoves()
    {
        return m_unprocessedMoves;
    }

    void ClearUnprocessedMoves()
    {
        m_unprocessedMoves.clear();
    }

public:
    std::shared_ptr<ReplicationManagerServer> m_replicationManager;
    std::shared_ptr<DeliveryManagerServer> m_deliveryManager;

    bool m_isLastMoveTimestampDirty;

private:
    SocketAddress m_socketAddress;
    std::string m_name;

    std::deque<Move> m_unprocessedMoves;
    F32 m_lastPacketTime;
};
}

#endif
