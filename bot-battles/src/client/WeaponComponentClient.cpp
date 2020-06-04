#include "WeaponComponent.h"

#include "ClientComponent.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "ReplicationManagerClient.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void WeaponComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType replicationActionType, Entity entity)
{
    /* TODO GAIN PRIMARY WEAPON
            if (replicationActionType != ReplicationActionType::CREATE) {
            Event newWeaponEvent;
            newWeaponEvent.eventType = EventType::WEAPON_PRIMARY_GAINED;
            newWeaponEvent.weapon.entity = entity;
            std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
            clientComponent.m_replicationManager.NotifyEvent(newWeaponEvent);
        }
    */
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_PRIMARY)) {
        inputStream.Read(m_damagePrimary);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_SECONDARY)) {
        inputStream.Read(m_damageSecondary);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_CURRENT_AMMO_PRIMARY)) {
        inputStream.Read(m_currentAmmoPrimary);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_MAX_AMMO_PRIMARY)) {
        inputStream.Read(m_maxAmmoPrimary);
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
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_TIME_SHOOT_PRIMARY)) {
        inputStream.Read(m_timeShootPrimary);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_SHOOT_PRIMARY)) {
        inputStream.Read(m_cooldownShootPrimary);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_TIME_SHOOT_SECONDARY)) {
        inputStream.Read(m_timeShootSecondary);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_SHOOT_SECONDARY)) {
        inputStream.Read(m_cooldownShootSecondary);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_TIME_RELOAD)) {
        inputStream.Read(m_timeReload);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_RELOAD)) {
        inputStream.Read(m_cooldownReload);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_ORIGIN_LAST_SHOT)) {
        inputStream.Read(m_originLastShot);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_DESTINATION_LAST_SHOT)) {
        inputStream.Read(m_destinationLastShot);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_HAS_HIT_LAST_SHOT)) {
        inputStream.Read(m_hasHitLastShot);

        if (replicationActionType != ReplicationActionType::CREATE) {
            Event newWeaponEvent;
            if (m_hasHitLastShot) {
                newWeaponEvent.eventType = EventType::WEAPON_HIT;
            } else {
                newWeaponEvent.eventType = EventType::WEAPON_MISSED;
            }
            newWeaponEvent.weapon.entity = entity;
            std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
            clientComponent.lock()->m_replicationManager.NotifyEvent(newWeaponEvent);
        }
    }
}
}
