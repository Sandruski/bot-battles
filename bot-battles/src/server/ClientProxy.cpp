#include "ClientProxy.h"

#include "ReplicationManagerServer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ClientProxy::ClientProxy(const SocketAddress& socketAddress, const char* name)
    : m_replicationManager(nullptr)
    , m_deliveryManager()
    , m_isLastMoveTimestampDirty(true)
    , m_socketAddress(socketAddress)
    , m_name(name)
    , m_unprocessedMoves()
    , m_lastPacketTime(0.0f)
{
    m_replicationManager = std::make_shared<ReplicationManagerServer>();
}

//----------------------------------------------------------------------------------------------------
const SocketAddress& ClientProxy::GetSocketAddress() const
{
    return m_socketAddress;
}

//----------------------------------------------------------------------------------------------------
const char* ClientProxy::GetName() const
{
    return m_name.c_str();
}

//----------------------------------------------------------------------------------------------------
void ClientProxy::UpdateLastPacketTime()
{
    m_lastPacketTime = Time::GetInstance().GetTime();
}

//----------------------------------------------------------------------------------------------------
F32 ClientProxy::GetLastPacketTime() const
{
    return m_lastPacketTime;
}

//----------------------------------------------------------------------------------------------------
const Move& ClientProxy::AddMove(const Move& move)
{
    float lastMoveTimestamp = GetLastMoveTimestamp();
    float timestamp = move.GetTimestamp();
    float dt = lastMoveTimestamp > 0.0f ? timestamp - lastMoveTimestamp : 0.0f;

    return m_unprocessedMoves.emplace_back(move.GetInput(), move.GetDirtyState(), timestamp, dt);
}

//----------------------------------------------------------------------------------------------------
F32 ClientProxy::GetLastMoveTimestamp() const
{
    return !m_unprocessedMoves.empty() ? m_unprocessedMoves.back().GetTimestamp() : 0.0f;
}

//----------------------------------------------------------------------------------------------------
const std::deque<Move>& ClientProxy::GetUnprocessedMoves()
{
    return m_unprocessedMoves;
}

//----------------------------------------------------------------------------------------------------
void ClientProxy::ClearUnprocessedMoves()
{
    m_unprocessedMoves.clear();
}
}
