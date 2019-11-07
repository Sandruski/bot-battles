#include "ClientProxy.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ClientProxy::ClientProxy()
{
}

//----------------------------------------------------------------------------------------------------
ClientProxy::ClientProxy(const SocketAddress& socketAddress, const char* name)
    : m_replicationManager()
    , m_socketAddress(socketAddress)
    , m_unprocessedMoves()
    , m_name(name)
    , m_lastPacketTime(0.0f)
    , m_isLastMoveTimestampDirty(true)
{
}

//----------------------------------------------------------------------------------------------------
ClientProxy::~ClientProxy()
{
}
//----------------------------------------------------------------------------------------------------
void ClientProxy::UpdateLastPacketTime()
{
    m_lastPacketTime = Time::GetInstance().GetTime();
}
}
