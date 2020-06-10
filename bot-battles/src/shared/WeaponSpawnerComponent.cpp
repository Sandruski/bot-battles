#include "WeaponSpawnerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WeaponSpawnerComponent::WeaponSpawnerComponent()
    : m_weaponDamage(0)
    , m_weaponMaxAmmo(0)
    , m_weaponAmmo(0)
    , m_weaponRange(0.0f)
    , m_weaponTimeShoot(0.0f)
    , m_weaponCooldownShoot(0.0f)
    , m_weaponTimeReload(0.0f)
    , m_weaponCooldownReload(0.0f)
    , m_amountSpawn(0)
    , m_cooldownSpawn(0.0f)
    , m_timerSpawn()
    , m_entitySpawned(INVALID_ENTITY)
    , m_amountSpawned(0)
{
}
}
