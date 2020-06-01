#ifndef __HEALTH_SPAWNER_COMPONENT_H__
#define __HEALTH_SPAWNER_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct HealthSpawnerComponent : public Component {

    static ComponentType GetType() { return ComponentType::HEALTH_SPAWNER; }

    HealthSpawnerComponent();

    // Local (server)
    U32 m_HP;
    F32 m_timeHeal;
    F32 m_cooldownHeal;

    I32 m_amountSpawn;
    F32 m_cooldownSpawn;

    MyTimer m_timerSpawn;
    Entity m_entitySpawned;
    U32 m_amountSpawned;
};
}

#endif
