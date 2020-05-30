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
    U32 m_damageWeapon1;
    U32 m_damageWeapon2;
    U32 m_ammoWeapon1;
    U32 m_ammoWeapon2;
    F32 m_rangeWeapon1;
    F32 m_rangeWeapon2;
    F32 m_cooldownWeapon1;
    F32 m_cooldownWeapon2;
    bool m_spawnWeapon1;
    bool m_spawnWeapon2;
    F32 m_timeoutSpawn;

    MyTimer m_timerSpawn;
    Entity m_entitySpawned;
};
}

#endif
