#ifndef __HEALTH_SPAWNER_COMPONENT_H__
#define __HEALTH_SPAWNER_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct HealthSpawnerComponent : public Component {

    static ComponentType GetType() { return ComponentType::HEALTH_SPAWNER; }

    HealthSpawnerComponent();

    U32 m_pointsHealth1;
    U32 m_pointsHealth2;
    bool m_spawnHealth1;
    bool m_spawnHealth2;
    F32 m_timeoutSpawn;

    MyTimer m_timerSpawn;
    Entity m_entitySpawned;
};
}

#endif
