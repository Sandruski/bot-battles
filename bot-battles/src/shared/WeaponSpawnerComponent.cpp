#include "WeaponSpawnerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WeaponSpawnerComponent::WeaponSpawnerComponent()
    : m_damageWeapon(0)
    , m_ammoWeapon(0)
    , m_ammo(0)
    , m_rangeWeapon(0.0f)
    , m_timeShoot(0.0f)
    , m_cooldownShoot(0.0f)
    , m_timeReload(0.0f)
    , m_cooldownReload(0.0f)
    , m_amountSpawn(0)
    , m_cooldownSpawn(0.0f)
    , m_timerSpawn()
    , m_entitySpawned(INVALID_ENTITY)
    , m_amountSpawned()
{
}
}
