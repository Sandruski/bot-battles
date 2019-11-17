#include "ClientProxy.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ClientProxy::ClientProxy()
{
}

//----------------------------------------------------------------------------------------------------
ClientProxy::ClientProxy(const SocketAddress& socketAddress, const char* name)
    : m_replicationManager()
    , m_entity()
    , m_socketAddress(socketAddress)
    , m_name(name)
    , m_unprocessedMoves()
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

//----------------------------------------------------------------------------------------------------
const Move& ClientProxy::AddMove(const Move& move)
{
    float lastMoveTimestamp = !m_unprocessedMoves.empty() ? m_unprocessedMoves.back().GetTimestamp() : 0.0f;
    float timestamp = move.GetTimestamp();
    float dt = lastMoveTimestamp > 0.0f ? timestamp - lastMoveTimestamp : 0.0f;

    return m_unprocessedMoves.emplace_back(move.GetInput(), timestamp, dt);
}
}
