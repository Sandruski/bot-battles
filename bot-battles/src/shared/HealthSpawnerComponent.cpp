#include "HealthSpawnerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthSpawnerComponent::HealthSpawnerComponent()
    : m_pointsHealth1(0)
    , m_pointsHealth2(0)
    , m_spawnHealth1(false)
    , m_spawnHealth2(false)
    , m_amountSpawn(0)
    , m_timeoutSpawn(0.0f)
    , m_timerSpawn()
    , m_entitySpawned(INVALID_ENTITY)
    , m_amountSpawned(0)
{
}
}
