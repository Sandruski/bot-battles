#ifndef __CLIENT_PROXY_H__
#define __CLIENT_PROXY_H__

#include "DeliveryManagerServer.h"
#include "Move.h"
#include "SocketAddress.h"

namespace sand {

class ReplicationManagerServer;

//----------------------------------------------------------------------------------------------------
class ClientProxy {
public:
    ClientProxy(const SocketAddress& socketAddress, const char* name);

    const SocketAddress& GetSocketAddress() const;
    const char* GetName() const;

    const Move& AddMove(const Move& move);
    F32 GetLastMoveTimestamp() const;
    const std::deque<Move>& GetUnprocessedMoves();
    void ClearUnprocessedMoves();

    void UpdateLastPacketTime();
    F32 GetLastPacketTime() const;

public:
    std::shared_ptr<ReplicationManagerServer> m_replicationManager;
    DeliveryManagerServer m_deliveryManager;

    bool m_isLastMoveTimestampDirty;

private:
    SocketAddress m_socketAddress;
    std::string m_name;

    std::deque<Move> m_unprocessedMoves;
    F32 m_lastPacketTime;
};
}

#endif
