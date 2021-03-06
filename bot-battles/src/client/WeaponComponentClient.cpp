#include "WeaponComponent.h"

#include "ClientComponent.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "ReplicationManagerClient.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void WeaponComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType replicationActionType, Entity entity)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_ORIGIN_LAST_SHOT)) {
        inputStream.Read(m_originLastShot);
        m_alpha = 1.0f;
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_DESTINATION_LAST_SHOT)) {
        inputStream.Read(m_destinationLastShot);
        m_alpha = 1.0f;
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_HIT_ENTITY_LAST_SHOT)) {
        inputStream.Read(m_hitEntityLastShot);
        m_alpha = 1.0f;

        if (replicationActionType != ReplicationActionType::CREATE) {
            Event newWeaponEvent;
            if (m_hitEntityLastShot == INVALID_ENTITY) {
                newWeaponEvent.eventType = EventType::WEAPON_MISSED;
            } else {
                newWeaponEvent.eventType = EventType::WEAPON_HIT;
                newWeaponEvent.weapon.targetEntity = m_hitEntityLastShot;
            }
            newWeaponEvent.weapon.shooterEntity = entity;
            std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
            clientComponent.lock()->m_replicationManager.NotifyEvent(newWeaponEvent);
        }
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_PRIMARY)) {
        inputStream.Read(m_damagePrimary);

        if (replicationActionType != ReplicationActionType::CREATE) {
            Event newWeaponEvent;
            newWeaponEvent.eventType = EventType::WEAPON_PRIMARY_PICKED_UP;
            newWeaponEvent.weapon.shooterEntity = entity;
            std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
            clientComponent.lock()->m_replicationManager.NotifyEvent(newWeaponEvent);
        }
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_SECONDARY)) {
        inputStream.Read(m_damageSecondary);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_CURRENT_AMMO_PRIMARY)) {
        I32 oldCurrentAmmoPrimary = m_currentAmmoPrimary;
        inputStream.Read(m_currentAmmoPrimary);

        if (replicationActionType != ReplicationActionType::CREATE) {
            const bool hasWeaponPrimaryPickedUp = dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_PRIMARY);
            if (!hasWeaponPrimaryPickedUp && oldCurrentAmmoPrimary < m_currentAmmoPrimary) {
                Event newWeaponEvent;
                newWeaponEvent.eventType = EventType::WEAPON_PRIMARY_RELOADED;
                newWeaponEvent.weapon.shooterEntity = entity;
                newWeaponEvent.weapon.ammo = m_currentAmmoPrimary - oldCurrentAmmoPrimary;
                std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
                clientComponent.lock()->m_replicationManager.NotifyEvent(newWeaponEvent);
            }
        }
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
}
}
