#include "WeaponSpawnerComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WeaponSpawnerComponent::WeaponSpawnerComponent()
    : m_weapon(0)
    , m_damageWeapon1(0)
    , m_damageWeapon2(0)
    , m_ammoWeapon1(0)
    , m_ammoWeapon2(0)
    , m_rangeWeapon1(0.0f)
    , m_rangeWeapon2(0.0f)
    , m_cooldownWeapon1(0.0f)
    , m_cooldownWeapon2(0.0f)
    , m_timer()
    , m_timeout(0.0f)
    , m_spawnWeapon1(false)
    , m_spawnWeapon2(false)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void WeaponSpawnerComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_WEAPON)) {
        inputStream.Read(m_weapon);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_DAMAGE_WEAPON1)) {
        inputStream.Read(m_damageWeapon1);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_DAMAGE_WEAPON2)) {
        inputStream.Read(m_damageWeapon2);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_AMMO_WEAPON1)) {
        inputStream.Read(m_ammoWeapon1);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_AMMO_WEAPON2)) {
        inputStream.Read(m_ammoWeapon2);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_RANGE_WEAPON1)) {
        inputStream.Read(m_rangeWeapon1);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_RANGE_WEAPON2)) {
        inputStream.Read(m_rangeWeapon2);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_COOLDOWN_WEAPON1)) {
        inputStream.Read(m_cooldownWeapon1);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_COOLDOWN_WEAPON2)) {
        inputStream.Read(m_cooldownWeapon2);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U64 WeaponSpawnerComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_WEAPON)) {
        outputStream.Write(m_weapon);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_WEAPON);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_DAMAGE_WEAPON1)) {
        outputStream.Write(m_damageWeapon1);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_DAMAGE_WEAPON1);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_DAMAGE_WEAPON2)) {
        outputStream.Write(m_damageWeapon2);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_DAMAGE_WEAPON2);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_AMMO_WEAPON1)) {
        outputStream.Write(m_ammoWeapon1);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_AMMO_WEAPON1);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_AMMO_WEAPON2)) {
        outputStream.Write(m_ammoWeapon2);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_AMMO_WEAPON2);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_RANGE_WEAPON1)) {
        outputStream.Write(m_rangeWeapon1);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_RANGE_WEAPON1);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_RANGE_WEAPON2)) {
        outputStream.Write(m_rangeWeapon2);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_RANGE_WEAPON2);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_COOLDOWN_WEAPON1)) {
        outputStream.Write(m_cooldownWeapon1);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_COOLDOWN_WEAPON1);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_COOLDOWN_WEAPON2)) {
        outputStream.Write(m_cooldownWeapon2);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_COOLDOWN_WEAPON2);
    }

    return writtenState;
}
#endif
}
