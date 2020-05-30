#ifndef __BOT_SPAWNER_COMPONENT_H__
#define __BOT_SPAWNER_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct BotSpawnerComponent : public Component {

    static ComponentType GetType() { return ComponentType::BOT_SPAWNER; }

    BotSpawnerComponent();

    // Local (server)
    F32 m_facing;
    F32 m_sightAngle;
    F32 m_sightDistance;
    PlayerID m_playerID;
    U32 m_health;

    U32 m_damageWeapon;
    F32 m_rangeWeapon;
    F32 m_cooldownWeapon;
};
}

#endif
