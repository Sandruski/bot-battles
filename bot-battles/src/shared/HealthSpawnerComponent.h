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
    bool m_hasHealth;
    U32 m_health;

    // Local (server)
    MyTimer m_timer;
    F32 m_timeout;
};
}

#endif
