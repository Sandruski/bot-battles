#ifndef __BOT_SPAWNER_COMPONENT_H__
#define __BOT_SPAWNER_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct BotSpawnerComponent : public Component {

    static ComponentType GetType() { return ComponentType::BOT_SPAWNER; }

    BotSpawnerComponent();

    // TODO: separate per components

    // Local (server)
    F32 m_facing;
    F32 m_maxLinearVelocity;
    F32 m_maxAngularVelocity;
    F32 m_sightAngle;
    F32 m_sightDistance;
    PlayerID m_playerID;
    U32 m_HP;

    U32 m_damageWeapon;
    F32 m_rangeWeapon;
    F32 m_timeShoot;
    F32 m_cooldownShoot;
};
}

#endif
