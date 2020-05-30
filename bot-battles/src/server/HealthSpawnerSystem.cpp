#include "HealthSpawnerSystem.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthSpawnerSystem::HealthSpawnerSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::HEALTH_SPAWNER);
}

//----------------------------------------------------------------------------------------------------
bool HealthSpawnerSystem::Update()
{
    return true;
}
}
