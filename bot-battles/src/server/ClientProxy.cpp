#include "ClientProxy.h"

#include "ReplicationManagerServer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ClientProxy::ClientProxy(const SocketAddress& socketAddress, const char* name)
    : m_replicationManager(nullptr)
    , m_deliveryManager()
    , m_inputBuffer()
    , m_lastAckdFrame(0)
    , m_timestamp(0.0f)
    , m_socketAddress(socketAddress)
    , m_name(name)
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
    m_lastPacketTime = MyTime::GetInstance().GetStartFrameTime();
}

//----------------------------------------------------------------------------------------------------
F32 ClientProxy::GetLastPacketTime() const
{
    return m_lastPacketTime;
}

//----------------------------------------------------------------------------------------------------
void ClientProxy::Reset()
{
    m_replicationManager->Reset();
    m_deliveryManager.Reset();
    m_inputBuffer.Clear();
    m_lastAckdFrame = 0;
    m_timestamp = 0.0f;
    m_lastPacketTime = 0.0f;
}
}
