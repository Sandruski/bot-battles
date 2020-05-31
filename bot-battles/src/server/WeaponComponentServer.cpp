#include "WeaponComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
U64 WeaponComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_WEAPON_PRIMARY)) {
        outputStream.Write(m_weaponPrimary);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_WEAPON_PRIMARY);
    }
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
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_HAS_SHOT)) {
        outputStream.Write(m_hasShot);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_HAS_SHOT);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_HAS_HIT)) {
        outputStream.Write(m_hasHit);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_HAS_HIT);
    }

    return writtenState;
}
}
