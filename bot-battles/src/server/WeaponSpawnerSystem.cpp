#include "WeaponSpawnerSystem.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "LinkingContext.h"
#include "ResourceManager.h"
#include "RigidbodyComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"
#include "WeaponSpawnerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
WeaponSpawnerSystem::WeaponSpawnerSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::WEAPON_SPAWNER);
}

//----------------------------------------------------------------------------------------------------
bool WeaponSpawnerSystem::Update()
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
            if (weaponSpawnerComponent.lock()->m_amountSpawn == 0 || (weaponSpawnerComponent.lock()->m_amountSpawn != -1 && static_cast<I32>(weaponSpawnerComponent.lock()->m_amountSpawned) >= weaponSpawnerComponent.lock()->m_amountSpawn)) {
                continue;
            }
            if (weaponSpawnerComponent.lock()->m_entitySpawned < INVALID_ENTITY) {
                continue;
            }
            if (weaponSpawnerComponent.lock()->m_timerSpawn.ReadSec() < weaponSpawnerComponent.lock()->m_cooldownSpawn) {
                continue;
            }

            weaponSpawnerComponent.lock()->m_entitySpawned = SpawnWeapon(entity);
            ++weaponSpawnerComponent.lock()->m_amountSpawned;

            weaponSpawnerComponent.lock()->m_timerSpawn.Start();
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void WeaponSpawnerSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::COLLISION_ENTER: {
        OnCollisionEnter(event.collision.entityA, event.collision.entityB);
        break;
    }

    case EventType::ENTITY_REMOVED: {
        OnEntityRemoved(event.entity.entity);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
Entity WeaponSpawnerSystem::SpawnWeapon(Entity spawner) const
{
    std::weak_ptr<WeaponSpawnerComponent> weaponSpawnerComponent = g_gameServer->GetComponentManager().GetComponent<WeaponSpawnerComponent>(spawner);
    std::weak_ptr<TransformComponent> weaponSpawnerTransformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(spawner);

    Entity entity = g_gameServer->GetEntityManager().AddEntity();
    g_gameServer->GetLinkingContext().AddEntity(entity);

    // Transform
    std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(entity);
    transformComponent.lock()->m_position = weaponSpawnerTransformComponent.lock()->m_position;
    transformComponent.lock()->m_layerType = LayerType::OBJECT;
    transformComponent.lock()->m_rotation = 0.0f;
    transformComponent.lock()->m_scale = 1.0f;

    // Sprite
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(entity);
    std::weak_ptr<SpriteResource> spriteResource = g_gameServer->GetResourceManager().AddResource<SpriteResource>("objects.png", TEXTURES_DIR, true);
    spriteComponent.lock()->m_spriteResource = spriteResource;
    glm::vec4 textureCoords = glm::vec4(1.0f, 1.0f, 33.0f, 10.0f);
    spriteComponent.lock()->AddSprite("default", textureCoords);
    spriteComponent.lock()->m_spriteName = "default";

    // Collider
    std::weak_ptr<ColliderComponent> colliderComponent = g_game->GetComponentManager().AddComponent<ColliderComponent>(entity);
    colliderComponent.lock()->m_size = glm::vec2(64.0f, 64.0f);
    colliderComponent.lock()->m_size *= transformComponent.lock()->m_scale;
    colliderComponent.lock()->m_shapeType = ColliderComponent::ShapeType::BOX;
    colliderComponent.lock()->m_isTrigger = true;

    // Rigidbody
    std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_game->GetComponentManager().AddComponent<RigidbodyComponent>(entity);
    rigidbodyComponent.lock()->m_bodyType = RigidbodyComponent::BodyType::STATIC;
    rigidbodyComponent.lock()->UpdateBodyType();
    rigidbodyComponent.lock()->m_groupIndex = 0;
    rigidbodyComponent.lock()->UpdateGroupIndex();

    // Weapon
    std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().AddComponent<WeaponComponent>(entity);
    weaponComponent.lock()->m_damagePrimary = weaponSpawnerComponent.lock()->m_damageWeapon;
    weaponComponent.lock()->m_maxAmmoPrimary = weaponSpawnerComponent.lock()->m_ammoWeapon;
    weaponComponent.lock()->m_ammoPrimary = weaponSpawnerComponent.lock()->m_ammo;
    weaponComponent.lock()->m_rangePrimary = weaponSpawnerComponent.lock()->m_rangeWeapon;
    weaponComponent.lock()->m_timeShootPrimary = weaponSpawnerComponent.lock()->m_timeShoot;
    weaponComponent.lock()->m_cooldownShootPrimary = weaponSpawnerComponent.lock()->m_cooldownShoot;
    weaponComponent.lock()->m_timeReload = weaponSpawnerComponent.lock()->m_timeReload;
    weaponComponent.lock()->m_cooldownReload = weaponSpawnerComponent.lock()->m_cooldownReload;

    return entity;
}

//----------------------------------------------------------------------------------------------------
void WeaponSpawnerSystem::DespawnWeapon(Entity entity) const
{
    g_gameServer->GetLinkingContext().RemoveEntity(entity);
    g_gameServer->GetEntityManager().RemoveEntity(entity);
}

//----------------------------------------------------------------------------------------------------
bool WeaponSpawnerSystem::PickUpWeapon(Entity character, Entity weapon) const
{
    std::weak_ptr<WeaponComponent> characterWeaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(character);
    std::weak_ptr<WeaponComponent> weaponWeaponComponent = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(weapon);

    U64 weaponDirtyState = 0;
    characterWeaponComponent.lock()->m_damagePrimary = weaponWeaponComponent.lock()->m_damagePrimary;
    weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_PRIMARY);
    characterWeaponComponent.lock()->m_maxAmmoPrimary = weaponWeaponComponent.lock()->m_maxAmmoPrimary;
    weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_MAX_AMMO_PRIMARY);
    characterWeaponComponent.lock()->m_ammoPrimary = weaponWeaponComponent.lock()->m_ammoPrimary;
    weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_AMMO_PRIMARY);
    characterWeaponComponent.lock()->Reload();
    weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_CURRENT_AMMO_PRIMARY);
    characterWeaponComponent.lock()->m_rangePrimary = weaponWeaponComponent.lock()->m_rangePrimary;
    weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_RANGE_PRIMARY);
    characterWeaponComponent.lock()->m_timeShootPrimary = weaponWeaponComponent.lock()->m_timeShootPrimary;
    weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_TIME_SHOOT_PRIMARY);
    characterWeaponComponent.lock()->m_cooldownShootPrimary = weaponWeaponComponent.lock()->m_cooldownShootPrimary;
    weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_SHOOT_PRIMARY);
    characterWeaponComponent.lock()->m_timeReload = weaponWeaponComponent.lock()->m_timeReload;
    weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_TIME_RELOAD);
    characterWeaponComponent.lock()->m_cooldownReload = weaponWeaponComponent.lock()->m_cooldownReload;
    weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_RELOAD);

    if (weaponDirtyState > 0) {
        Event newComponentEvent;
        newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
        newComponentEvent.component.entity = character;
        newComponentEvent.component.dirtyState = weaponDirtyState;
        NotifyEvent(newComponentEvent);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void WeaponSpawnerSystem::OnCollisionEnter(Entity entityA, Entity entityB) const
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    PlayerID playerIDA = serverComponent.GetPlayerID(entityA);
    PlayerID playerIDB = serverComponent.GetPlayerID(entityB);
    if ((playerIDA < INVALID_PLAYER_ID && playerIDB < INVALID_PLAYER_ID)
        || (playerIDA >= INVALID_PLAYER_ID && playerIDB >= INVALID_PLAYER_ID)) {
        return;
    }

    if (playerIDA < INVALID_PLAYER_ID) {
        std::weak_ptr<WeaponComponent> weaponComponentB = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entityB);
        if (!weaponComponentB.expired()) {
            const bool isPickUp = PickUpWeapon(entityA, entityB);
            if (isPickUp) {
                DespawnWeapon(entityB);
            }
        }
    } else if (playerIDB < INVALID_PLAYER_ID) {
        std::weak_ptr<WeaponComponent> weaponComponentA = g_gameServer->GetComponentManager().GetComponent<WeaponComponent>(entityA);
        if (!weaponComponentA.expired()) {
            const bool isPickUp = PickUpWeapon(entityB, entityA);
            if (isPickUp) {
                DespawnWeapon(entityA);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void WeaponSpawnerSystem::OnEntityRemoved(Entity entityRemoved) const
{
    for (auto& entity : m_entities) {
        std::weak_ptr<WeaponSpawnerComponent> weaponSpawnerComponent = g_gameServer->GetComponentManager().GetComponent<WeaponSpawnerComponent>(entity);
        if (!weaponSpawnerComponent.expired()) {
            if (weaponSpawnerComponent.lock()->m_entitySpawned == entityRemoved) {
                weaponSpawnerComponent.lock()->m_entitySpawned = INVALID_ENTITY;
                break;
            }
        }
    }
}
}
