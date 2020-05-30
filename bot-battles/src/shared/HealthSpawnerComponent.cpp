#include "HealthSpawnerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthSpawnerComponent::HealthSpawnerComponent()
    : m_hasHealth(false)
    , m_health(0)
    , m_timer()
    , m_timeout(0)
{
}
}
