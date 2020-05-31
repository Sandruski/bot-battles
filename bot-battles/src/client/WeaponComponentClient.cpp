#include "WeaponComponent.h"

#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "ReplicationManagerClient.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void WeaponComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType replicationActionType, Entity entity)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_WEAPON_PRIMARY)) {
        inputStream.Read(m_weaponPrimary);

        if (replicationActionType != ReplicationActionType::CREATE) {
            Event newWeaponEvent;
            newWeaponEvent.eventType = EventType::WEAPON_PRIMARY_GAINED;
            newWeaponEvent.weapon.entity = entity;
            ClientComponent& clientComponent = g_gameClient->GetClientComponent();
            clientComponent.m_replicationManager.NotifyEvent(newWeaponEvent);
        }
    }
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
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_HAS_SHOT)) {
        inputStream.Read(m_hasShot);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::WEAPON_HAS_HIT)) {
        inputStream.Read(m_hasHit);

        if (replicationActionType != ReplicationActionType::CREATE) {
            Event newWeaponEvent;
            if (m_hasHit) {
                newWeaponEvent.eventType = EventType::WEAPON_HIT;
            } else {
                newWeaponEvent.eventType = EventType::WEAPON_MISSED;
            }
            newWeaponEvent.weapon.entity = entity;
            ClientComponent& clientComponent = g_gameClient->GetClientComponent();
            clientComponent.m_replicationManager.NotifyEvent(newWeaponEvent);
        }
    }
}
}
