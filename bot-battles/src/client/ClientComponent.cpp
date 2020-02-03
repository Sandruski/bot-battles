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
    , m_socket(nullptr)
    , m_socketAddress(nullptr)
    , m_name()
    , m_ip()
    , m_port()
    , m_playerID(INVALID_PLAYER_ID)
    , m_entity(INVALID_ENTITY)
    , m_lastAckdFrame(0)
    , m_inputBuffer()
    , m_isLastMovePending(false)
    , m_RTT(0.0f)
    , m_isClientSidePrediction(false)
    , m_isServerReconciliation(false)
    , m_isEntityInterpolation(false)
    , m_isLagCompensation(false)
{
}

//----------------------------------------------------------------------------------------------------
void ClientComponent::LoadFromConfig(const rapidjson::Value& value)
{
    assert(value.HasMember("name"));
    assert(value["name"].IsString());
    m_name = value["name"].GetString();

    assert(value.HasMember("ip"));
    assert(value["ip"].IsString());
    m_ip = value["ip"].GetString();

    assert(value.HasMember("port"));
    assert(value["port"].IsString());
    m_port = value["port"].GetString();

    assert(value.HasMember("clientSidePrediction"));
    assert(value["clientSidePrediction"].IsBool());
    m_isClientSidePrediction = value["clientSidePrediction"].GetBool();

    assert(value.HasMember("serverReconciliation"));
    assert(value["serverReconciliation"].IsBool());
    m_isServerReconciliation = value["serverReconciliation"].GetBool();

    assert(value.HasMember("entityInterpolation"));
    assert(value["entityInterpolation"].IsBool());
    m_isEntityInterpolation = value["entityInterpolation"].GetBool();

    assert(value.HasMember("lagCompensation"));
    assert(value["lagCompensation"].IsBool());
    m_isLagCompensation = value["lagCompensation"].GetBool();
}

//----------------------------------------------------------------------------------------------------
bool ClientComponent::IsConnected() const
{
    return m_playerID != INVALID_PLAYER_ID;
}

//----------------------------------------------------------------------------------------------------
bool ClientComponent::IsLocalPlayer(Entity entity) const
{
    return entity == m_entity;
}
}
