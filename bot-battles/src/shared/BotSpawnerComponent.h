#ifndef __BOT_SPAWNER_COMPONENT_H__
#define __BOT_SPAWNER_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct BotSpawnerComponent : public Component {

    static ComponentType GetType() { return ComponentType::BOT_SPAWNER; }

    BotSpawnerComponent();

#ifdef _SERVER
    // Local (server)
    PlayerID m_playerID;

    /// Transform
    F32 m_transformRotation;

    /// Rigidbody
    F32 m_rigidbodyMaxLinearVelocity;
    F32 m_rigidbodyMaxAngularVelocity;

    /// Weapon (secondary)
    U32 m_weaponDamage;
    F32 m_weaponRange;
    F32 m_weaponTimeShoot;
    F32 m_weaponCooldownShoot;

    /// Health
    U32 m_healthMaxHP;

    /// Sight
    F32 m_sightAngle;
    F32 m_sightDistance;
#endif
};
}

#endif
