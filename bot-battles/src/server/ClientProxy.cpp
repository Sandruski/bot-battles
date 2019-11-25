#include "ClientProxy.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ClientProxy::ClientProxy()
{
}

//----------------------------------------------------------------------------------------------------
ClientProxy::ClientProxy(const SocketAddress& socketAddress, const char* name)
    : m_replicationManager()
    , m_deliveryManager()
    , m_socketAddress(socketAddress)
    , m_name(name)
    , m_unprocessedMoves()
    , m_lastPacketTime(0.0f)
    , m_isLastMoveTimestampDirty(true)
{
    m_deliveryManager.AddObserver(std::make_shared<ReplicationManagerServer>(m_replicationManager)); // TODO!!!
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

    return m_unprocessedMoves.emplace_back(move.GetInput(), move.GetDirtyState(), timestamp, dt);
}
}
