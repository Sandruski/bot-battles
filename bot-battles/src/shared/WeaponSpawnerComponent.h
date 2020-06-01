#ifndef __WEAPON_SPAWNER_COMPONENT_H__
#define __WEAPON_SPAWNER_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct WeaponSpawnerComponent : public Component {

    static ComponentType GetType() { return ComponentType::WEAPON_SPAWNER; }

    WeaponSpawnerComponent();

    // Local (server)
    U32 m_damageWeapon;
    U32 m_ammoWeapon;
    U32 m_ammo;
    F32 m_rangeWeapon;
    F32 m_timeShoot;
    F32 m_cooldownShoot;
    F32 m_timeReload;
    F32 m_cooldownReload;

    I32 m_amountSpawn;
    F32 m_cooldownSpawn;

    MyTimer m_timerSpawn;
    Entity m_entitySpawned;
    U32 m_amountSpawned;
};
}

#endif
