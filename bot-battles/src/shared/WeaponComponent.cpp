#include "WeaponComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WeaponComponent::WeaponComponent()
    : m_damagePrimary(0)
    , m_damageSecondary(0)
    , m_ammoPrimary(0)
    , m_rangePrimary(0.0f)
    , m_rangeSecondary(0.0f)
    , m_cooldownPrimary(0.0f)
    , m_cooldownSecondary(0.0f)
    , m_timestampLastShotPrimary(0.0f)
    , m_timestampLastShotSecondary(0.0f)
    , m_originShot(0.0f, 0.0f)
    , m_destinationShot(0.0f, 0.0f)
    , m_timeoutShot(0.1f)
    , m_timerShot()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void WeaponComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_PRIMARY)) {
        inputStream.Read(m_damagePrimary);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_SECONDARY)) {
        inputStream.Read(m_damageSecondary);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_AMMO_PRIMARY)) {
        inputStream.Read(m_ammoPrimary);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_RANGE_PRIMARY)) {
        inputStream.Read(m_rangePrimary);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_RANGE_SECONDARY)) {
        inputStream.Read(m_rangeSecondary);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_PRIMARY)) {
        inputStream.Read(m_cooldownPrimary);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_SECONDARY)) {
        inputStream.Read(m_cooldownSecondary);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U64 WeaponComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_PRIMARY)) {
        outputStream.Write(m_damagePrimary);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_PRIMARY);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_SECONDARY)) {
        outputStream.Write(m_damageSecondary);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_SECONDARY);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_AMMO_PRIMARY)) {
        outputStream.Write(m_ammoPrimary);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_AMMO_PRIMARY);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_RANGE_PRIMARY)) {
        outputStream.Write(m_rangePrimary);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_RANGE_PRIMARY);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_RANGE_SECONDARY)) {
        outputStream.Write(m_rangeSecondary);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_RANGE_SECONDARY);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_PRIMARY)) {
        outputStream.Write(m_cooldownPrimary);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_PRIMARY);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_SECONDARY)) {
        outputStream.Write(m_cooldownSecondary);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_SECONDARY);
    }

    return writtenState;
}
#endif
}
