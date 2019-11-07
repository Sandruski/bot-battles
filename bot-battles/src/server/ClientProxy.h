#ifndef __CLIENT_PROXY_H__
#define __CLIENT_PROXY_H__

#include "MoveList.h"
#include "ServerReplicationManager.h"
#include "SocketAddress.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class ClientProxy {
public:
    ClientProxy();
    ClientProxy(const char* name);
    ~ClientProxy();

    void UpdateLastPacketTime();

    MoveList& GetUnprocessedMoveList()
    {
        return m_unprocessedMoves;
    }

private:
    ServerReplicationManager m_replicationManager;
    MoveList m_unprocessedMoves;
    std::string m_name;
    F32 m_lastPacketTime;
    bool m_isLastMoveTimestampDirty;
};
}

#endif
