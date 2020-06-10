#include "BotSpawnerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
BotSpawnerComponent::BotSpawnerComponent()
    : m_facing(0.0f)
    , m_maxLinearVelocity(0.0f)
    , m_maxAngularVelocity(0.0f)
    , m_sightAngle(0.0f)
    , m_sightDistance(0.0f)
    , m_playerID(INVALID_PLAYER_ID)
    , m_HP(0)
    , m_damageWeapon(0)
    , m_rangeWeapon(0.0f)
    , m_timeShoot(0.0f)
    , m_cooldownShoot(0.0f)
{
}
}
