#include "ClientProxy.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ClientProxy::ClientProxy()
{
}

//----------------------------------------------------------------------------------------------------
ClientProxy::ClientProxy(const SocketAddress& socketAddress, const std::string& name, U32 playerID)
    : m_replicationManager()
    , m_unprocessedMoves()
    , m_socketAddress(socketAddress)
    , m_name(name)
    , m_playerID(playerID)
    , m_isLastMoveTimestampDirty(true)
{
}

//----------------------------------------------------------------------------------------------------
ClientProxy::~ClientProxy()
{
}
}
