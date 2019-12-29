#include "ClientComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ClientComponent::ClientComponent()
    : m_socket(nullptr)
    , m_socketAddress(nullptr)
    , m_name("ExamplePlayer")
    , m_playerID(INVALID_PLAYER_ID)
    , m_helloTime(1.0f)
    , m_inputTime(0.03f)
    , m_lastTime(0.0f)
{
}

//----------------------------------------------------------------------------------------------------
bool ClientComponent::IsConnected() const
{
    return m_playerID != INVALID_PLAYER_ID;
}

//----------------------------------------------------------------------------------------------------
F32 ClientComponent::GetNextHelloTime() const
{
    return m_lastTime + m_helloTime;
}

//----------------------------------------------------------------------------------------------------
F32 ClientComponent::GetNextInputTime() const
{
    return m_lastTime + m_inputTime;
}
}
