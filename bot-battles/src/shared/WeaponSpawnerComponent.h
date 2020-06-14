#ifndef __WEAPON_SPAWNER_COMPONENT_H__
#define __WEAPON_SPAWNER_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct WeaponSpawnerComponent : public Component {

    static ComponentType GetType() { return ComponentType::WEAPON_SPAWNER; }

    WeaponSpawnerComponent();

#ifdef _SERVER
    // Local (server)
    /// Weapon (primary)
    U32 m_weaponDamage;
    U32 m_weaponMaxAmmo;
    U32 m_weaponAmmo;
    F32 m_weaponRange;
    F32 m_weaponTimeShoot;
    F32 m_weaponCooldownShoot;
    F32 m_weaponTimeReload;
    F32 m_weaponCooldownReload;

    I32 m_amountSpawn;
    F32 m_cooldownSpawn;

    MyTimer m_timerSpawn;
    Entity m_entitySpawned;
    U32 m_amountSpawned;
#endif
};
}

#endif
