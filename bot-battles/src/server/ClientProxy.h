#ifndef __CLIENT_PROXY_H__
#define __CLIENT_PROXY_H__

#include "DeliveryManagerServer.h"
#include "Move.h"
#include "ReplicationManagerServer.h"
#include "SocketAddress.h"

namespace sand {

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

    DeliveryManagerServer& GetDeliveryManagerServer()
    {
        return m_deliveryManagerServer;
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
    bool m_isLastMoveTimestampDirty;
    std::shared_ptr<ReplicationManagerServer> m_replicationManagerServer;

private:
    DeliveryManagerServer m_deliveryManagerServer;

    SocketAddress m_socketAddress;
    std::string m_name;

    std::deque<Move> m_unprocessedMoves;
    F32 m_lastPacketTime;
};
}

#endif
