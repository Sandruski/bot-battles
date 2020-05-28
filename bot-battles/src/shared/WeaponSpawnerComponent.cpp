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
    , m_spawnWeapon1(false)
    , m_spawnWeapon2(false)
    , m_timeout(0.0f)
    , m_timer()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void WeaponSpawnerComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::WEAPON_SPAWNER_WEAPON)) {
        inputStream.Read(m_weapon);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 WeaponSpawnerComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::WEAPON_SPAWNER_WEAPON)) {
        outputStream.Write(m_weapon);
        writtenState |= static_cast<U32>(ComponentMemberType::WEAPON_SPAWNER_WEAPON);
    }

    return writtenState;
}
#endif
}
