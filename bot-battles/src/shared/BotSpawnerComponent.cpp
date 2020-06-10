#include "BotSpawnerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
BotSpawnerComponent::BotSpawnerComponent()
    : m_playerID(INVALID_PLAYER_ID)
    , m_transformRotation(0.0f)
    , m_rigidbodyMaxLinearVelocity(0.0f)
    , m_rigidbodyMaxAngularVelocity(0.0f)
    , m_weaponDamage(0)
    , m_weaponRange(0.0f)
    , m_weaponTimeShoot(0.0f)
    , m_weaponCooldownShoot(0.0f)
    , m_healthMaxHP(0)
    , m_sightAngle(0.0f)
    , m_sightDistance(0.0f)
{
}
}
