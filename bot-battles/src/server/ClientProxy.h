#ifndef __CLIENT_PROXY_H__
#define __CLIENT_PROXY_H__

#include "MoveList.h"
#include "ReplicationManager.h"
#include "SocketAddress.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class ClientProxy {
public:
    ClientProxy();
    ClientProxy(const SocketAddress& socketAddress, const std::string& name, U32 playerID);
    ~ClientProxy();

    MoveList& GetUnprocessedMoveList()
    {
        return m_unprocessedMoves;
    }

private:
    ReplicationManager m_replicationManager; // If I only have one client, this doesn't need to be here
    MoveList m_unprocessedMoves;
    SocketAddress m_socketAddress;
    std::string m_name;
    U32 m_playerID;
    bool m_isLastMoveTimestampDirty;
};
}

#endif
