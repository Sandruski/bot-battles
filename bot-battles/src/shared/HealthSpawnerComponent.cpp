#include "HealthSpawnerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthSpawnerComponent::HealthSpawnerComponent()
    : m_HP(0)
    , m_timeHeal(0.0f)
    , m_cooldownHeal(0.0f)
    , m_amountSpawn(0)
    , m_cooldownSpawn(0.0f)
    , m_timerSpawn()
    , m_entitySpawned(INVALID_ENTITY)
    , m_amountSpawned(0)
{
}
}
