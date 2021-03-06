#ifndef __HEALTH_SPAWNER_COMPONENT_H__
#define __HEALTH_SPAWNER_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct HealthSpawnerComponent : public Component {

    static ComponentType GetType() { return ComponentType::HEALTH_SPAWNER; }

    HealthSpawnerComponent();

#ifdef _SERVER
    // Local (server)
    /// Health
    U32 m_healthHP;
    F32 m_healthTimeHeal;
    F32 m_healthCooldownHeal;

    I32 m_amountSpawn;
    F32 m_cooldownSpawn;

    MyTimer m_timerSpawn;
    Entity m_entitySpawned;
    U32 m_amountSpawned;
#endif
};
}

#endif
