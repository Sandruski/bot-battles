#include "PickUpSystem.h"

#include "ClientProxy.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "LinkingContext.h"
#include "SpriteComponent.h"
#include "State.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "WeaponSpawnerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
PickUpSystem::PickUpSystem()
{
    // Spawners
    m_signature |= 1 << static_cast<U16>(ComponentType::SPAWNER);
}

// TODO: init timer

//----------------------------------------------------------------------------------------------------
bool PickUpSystem::Update()
{
    OPTICK_EVENT();

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    for (auto& entity : m_entities) {
        std::weak_ptr<WeaponSpawnerComponent> weaponSpawnerComponent = g_gameServer->GetComponentManager().GetComponent<WeaponSpawnerComponent>(entity);
        if (!weaponSpawnerComponent.expired()) {
            if (!weaponSpawnerComponent.lock()->m_spawnWeapon1 && !weaponSpawnerComponent.lock()->m_spawnWeapon2) {
                continue;
            }
            if (weaponSpawnerComponent.lock()->m_weapon > 0) {
                continue;
            }
            if (weaponSpawnerComponent.lock()->m_timer.ReadSec() < weaponSpawnerComponent.lock()->m_timeout) {
                continue;
            }
            weaponSpawnerComponent.lock()->m_timer.Start();
            U32 weapon1 = weaponSpawnerComponent.lock()->m_spawnWeapon1 ? 1 : 2;
            U32 weapon2 = weaponSpawnerComponent.lock()->m_spawnWeapon2 ? 2 : 1;
            weaponSpawnerComponent.lock()->m_weapon = RandomInt(weapon1, weapon2);

            Event newComponentEvent;
            newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
            newComponentEvent.component.entity = entity;
            newComponentEvent.component.dirtyState = static_cast<U64>(ComponentMemberType::WEAPON_SPAWNER_WEAPON);
            NotifyEvent(newComponentEvent);
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void PickUpSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::COLLISION_ENTER: {
        OnCollisionEnter(event.collision.entityA, event.collision.entityB);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void PickUpSystem::OnCollisionEnter(Entity entityA, Entity entityB) const
{
    Entity entity = INVALID_ENTITY;
    std::weak_ptr<WeaponSpawnerComponent> weaponSpawnerComponent;

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    PlayerID playerIDA = serverComponent.GetPlayerID(entityA);
    PlayerID playerIDB = serverComponent.GetPlayerID(entityB);
    if (playerIDA < INVALID_PLAYER_ID) {
        entity = entityA;
        std::weak_ptr<WeaponSpawnerComponent> weaponSpawnerComponentB = g_gameServer->GetComponentManager().GetComponent<WeaponSpawnerComponent>(entityB);
        if (!weaponSpawnerComponentB.expired()) {
            weaponSpawnerComponent = weaponSpawnerComponentB;
        }
    } else if (playerIDB < INVALID_PLAYER_ID) {
        entity = entityB;
        std::weak_ptr<WeaponSpawnerComponent> weaponSpawnerComponentA = g_gameServer->GetComponentManager().GetComponent<WeaponSpawnerComponent>(entityA);
        if (!weaponSpawnerComponentA.expired()) {
            weaponSpawnerComponent = weaponSpawnerComponentA;
        }
    }

    if (entity >= INVALID_ENTITY) {
        return;
    }

    if (!weaponSpawnerComponent.expired()) {
        U64 weaponDirtyState = 0;

        std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entity);
        switch (weaponSpawnerComponent.lock()->m_weapon) {
        case 0: {
            break;
        }
        case 1: {
            weaponComponent.lock()->m_damagePrimary = weaponSpawnerComponent.lock()->m_damageWeapon1;
            weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_PRIMARY);
            weaponComponent.lock()->m_ammoPrimary = weaponSpawnerComponent.lock()->m_ammoWeapon1;
            weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_AMMO_PRIMARY);
            weaponComponent.lock()->m_rangePrimary = weaponSpawnerComponent.lock()->m_rangeWeapon1;
            weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_RANGE_PRIMARY);
            weaponComponent.lock()->m_cooldownPrimary = weaponSpawnerComponent.lock()->m_cooldownWeapon1;
            weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_PRIMARY);
            break;
        }
        case 2: {
            weaponComponent.lock()->m_damagePrimary = weaponSpawnerComponent.lock()->m_damageWeapon2;
            weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_PRIMARY);
            weaponComponent.lock()->m_ammoPrimary = weaponSpawnerComponent.lock()->m_ammoWeapon2;
            weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_AMMO_PRIMARY);
            weaponComponent.lock()->m_rangePrimary = weaponSpawnerComponent.lock()->m_rangeWeapon2;
            weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_RANGE_PRIMARY);
            weaponComponent.lock()->m_cooldownPrimary = weaponSpawnerComponent.lock()->m_cooldownWeapon2;
            weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_PRIMARY);
            break;
        }
        default: {
            break;
        }
        }

        if (weaponDirtyState > 0) {
            Event newComponentEvent;
            newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
            newComponentEvent.component.entity = entity;
            newComponentEvent.component.dirtyState = weaponDirtyState;
            NotifyEvent(newComponentEvent);
        }
    }
}
}
