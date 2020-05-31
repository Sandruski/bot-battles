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
    // TODO: spawn only once or forever
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
            if (weaponSpawnerComponent.lock()->m_amountSpawn == 0 || (weaponSpawnerComponent.lock()->m_amountSpawn != -1 && static_cast<I32>(weaponSpawnerComponent.lock()->m_amountSpawned) >= weaponSpawnerComponent.lock()->m_amountSpawn)) {
                continue;
            }
            if (weaponSpawnerComponent.lock()->m_entitySpawned < INVALID_ENTITY) {
                continue;
            }
            if (weaponSpawnerComponent.lock()->m_timerSpawn.ReadSec() < weaponSpawnerComponent.lock()->m_timeoutSpawn) {
                continue;
            }
            weaponSpawnerComponent.lock()->m_timerSpawn.Start();

            U32 weapon1 = weaponSpawnerComponent.lock()->m_spawnWeapon1 ? 1 : 2;
            U32 weapon2 = weaponSpawnerComponent.lock()->m_spawnWeapon2 ? 2 : 1;
            U32 weapon = RandomInt(weapon1, weapon2);
            weaponSpawnerComponent.lock()->m_entitySpawned = SpawnWeapon(weapon, entity);
            ++weaponSpawnerComponent.lock()->m_amountSpawned;
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
Entity WeaponSpawnerSystem::SpawnWeapon(U32 weapon, Entity spawner) const
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
    switch (weapon) {
    case 1: {
        glm::vec4 textureCoords = glm::vec4(1.0f, 1.0f, 33.0f, 10.0f);
        spriteComponent.lock()->AddSprite("default", textureCoords);
        break;
    }
    case 2: {
        glm::vec4 textureCoords = glm::vec4(35.0f, 1.0f, 25.0f, 10.0f);
        spriteComponent.lock()->AddSprite("default", textureCoords);
        break;
    }
    default: {
        break;
    }
    }
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
    weaponComponent.lock()->m_weaponPrimary = weapon;
    switch (weapon) {
    case 1: {
        weaponComponent.lock()->m_damagePrimary = weaponSpawnerComponent.lock()->m_damageWeapon1;
        weaponComponent.lock()->m_ammoPrimary = weaponSpawnerComponent.lock()->m_ammoWeapon1;
        weaponComponent.lock()->m_rangePrimary = weaponSpawnerComponent.lock()->m_rangeWeapon1;
        weaponComponent.lock()->m_cooldownPrimary = weaponSpawnerComponent.lock()->m_cooldownWeapon1;
        break;
    }
    case 2: {
        weaponComponent.lock()->m_damagePrimary = weaponSpawnerComponent.lock()->m_damageWeapon2;
        weaponComponent.lock()->m_ammoPrimary = weaponSpawnerComponent.lock()->m_ammoWeapon2;
        weaponComponent.lock()->m_rangePrimary = weaponSpawnerComponent.lock()->m_rangeWeapon2;
        weaponComponent.lock()->m_cooldownPrimary = weaponSpawnerComponent.lock()->m_cooldownWeapon2;
        break;
    }
    default: {
        break;
    }
    }

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
    characterWeaponComponent.lock()->m_weaponPrimary = weaponWeaponComponent.lock()->m_weaponPrimary;
    weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_WEAPON_PRIMARY);
    characterWeaponComponent.lock()->m_damagePrimary = weaponWeaponComponent.lock()->m_damagePrimary;
    weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_DAMAGE_PRIMARY);
    characterWeaponComponent.lock()->m_ammoPrimary = weaponWeaponComponent.lock()->m_ammoPrimary;
    weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_AMMO_PRIMARY);
    characterWeaponComponent.lock()->m_rangePrimary = weaponWeaponComponent.lock()->m_rangePrimary;
    weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_RANGE_PRIMARY);
    characterWeaponComponent.lock()->m_cooldownPrimary = weaponWeaponComponent.lock()->m_cooldownPrimary;
    weaponDirtyState |= static_cast<U64>(ComponentMemberType::WEAPON_COOLDOWN_PRIMARY);

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
