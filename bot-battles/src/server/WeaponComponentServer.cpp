#include "WeaponComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

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
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_CURRENT_AMMO_PRIMARY)) {
        outputStream.Write(m_currentAmmoPrimary);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_CURRENT_AMMO_PRIMARY);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_MAX_AMMO_PRIMARY)) {
        outputStream.Write(m_maxAmmoPrimary);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_MAX_AMMO_PRIMARY);
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
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_TIME_SHOOT_PRIMARY)) {
        outputStream.Write(m_timeShootPrimary);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_TIME_SHOOT_PRIMARY);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_SHOOT_PRIMARY)) {
        outputStream.Write(m_cooldownShootPrimary);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_SHOOT_PRIMARY);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_TIME_SHOOT_SECONDARY)) {
        outputStream.Write(m_timeShootSecondary);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_TIME_SHOOT_SECONDARY);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_SHOOT_SECONDARY)) {
        outputStream.Write(m_cooldownShootSecondary);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_SHOOT_SECONDARY);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_TIME_RELOAD)) {
        outputStream.Write(m_timeReload);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_TIME_RELOAD);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_RELOAD)) {
        outputStream.Write(m_cooldownReload);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_RELOAD);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_ORIGIN_LAST_SHOT)) {
        outputStream.Write(m_originLastShot);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_ORIGIN_LAST_SHOT);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_DESTINATION_LAST_SHOT)) {
        outputStream.Write(m_destinationLastShot);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_DESTINATION_LAST_SHOT);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_HAS_HIT_LAST_SHOT)) {
        outputStream.Write(m_hasHitLastShot);
        writtenState |= static_cast<U64>(ComponentMemberType::WEAPON_HAS_HIT_LAST_SHOT);
    }

    return writtenState;
}
}
