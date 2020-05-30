#include "WeaponSpawnerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WeaponSpawnerComponent::WeaponSpawnerComponent()
    : m_damageWeapon1(0)
    , m_damageWeapon2(0)
    , m_ammoWeapon1(0)
    , m_ammoWeapon2(0)
    , m_rangeWeapon1(0.0f)
    , m_rangeWeapon2(0.0f)
    , m_cooldownWeapon1(0.0f)
    , m_cooldownWeapon2(0.0f)
    , m_spawnWeapon1(false)
    , m_spawnWeapon2(false)
    , m_timeoutSpawn(0.0f)
    , m_timerSpawn()
    , m_entitySpawned(INVALID_ENTITY)
{
}
}
