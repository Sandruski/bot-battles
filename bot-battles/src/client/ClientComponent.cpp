#include "ClientComponent.h"

namespace sand {

/* 
- Send all inputs to the server until they are ackd.
- No inputs left sent by a client to the server. What should the server do, freeze the game?

- Client Prediction + Rollback if there is a misprediction (causes a divergence).
- Buffer with states + Simulate forward + Blend.

- Player's perspective, authority's perspective, enemy's perspective.

// Interpolation (linear) for trajectory, for animation, and for life bars
// Teleportation: if the divergence is huge (or very fast interpolation)
*/

//----------------------------------------------------------------------------------------------------
ClientComponent::ClientComponent()
    : m_replicationManager()
    , m_deliveryManager()
    , m_UDPSocket()
    , m_TCPSocket()
    , m_socketAddress()
    , m_ip()
    , m_port()
    , m_map()
    , m_name()
    , m_playerID(INVALID_PLAYER_ID)
    , m_entity(INVALID_ENTITY)
    , m_lastPacketTime(0.0f)
    , m_lastAckdFrame(0)
    , m_interpolationFromFrame(0)
    , m_interpolationToFrame(0)
    , m_interpolationPercentage(0.0f)
    , m_entityInterpolationPeriod(0.0f)
    , m_frameBuffer()
    , m_inputBuffer()
    , m_isLastInputTransformPending(false)
    , m_isLastInputWeaponPending(false)
    , m_RTT(0.0f)
    , m_connectTimer()
    , m_isClientPredictionServerReconciliation(true)
    , m_isClientPrediction(true)
    , m_isServerReconciliation(true)
    , m_isEntityInterpolation(true)
{
}

//----------------------------------------------------------------------------------------------------
void ClientComponent::LoadFromConfig(const rapidjson::Value& value)
{
    assert(value.HasMember("ip"));
    m_ip = value["ip"].GetString();

    assert(value.HasMember("port"));
    m_port = value["port"].GetString();

    assert(value.HasMember("defaultName"));
    m_name = value["defaultName"].GetString();

    assert(value.HasMember("serverFps"));
    F32 serverFps = value["serverFps"].GetFloat();
    m_entityInterpolationPeriod = 1.0f / serverFps;
}

//----------------------------------------------------------------------------------------------------
bool ClientComponent::IsConnected() const
{
    return m_playerID < INVALID_PLAYER_ID;
}

//----------------------------------------------------------------------------------------------------
bool ClientComponent::IsLocalEntity(Entity entity) const
{
    return entity == m_entity;
}

//----------------------------------------------------------------------------------------------------
bool ClientComponent::IsLocalPlayer(PlayerID playerID) const
{
    return playerID == m_playerID;
}

//----------------------------------------------------------------------------------------------------
void ClientComponent::Reset()
{
    m_deliveryManager.Reset();
    m_lastPacketTime = 0.0f;
    m_lastAckdFrame = 0;
    m_interpolationFromFrame = 0;
    m_interpolationToFrame = 0;
    m_interpolationPercentage = 0.0f;
    m_frameBuffer.Clear();
    m_inputBuffer.Clear();
    m_isLastInputTransformPending = false;
    m_isLastInputWeaponPending = false;
    m_RTT = 0.0f;
}
}
