#include "HealthSpawnerSystem.h"

#include "BotComponent.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "HealthComponent.h"
#include "HealthSpawnerComponent.h"
#include "LinkingContext.h"
#include "ResourceManager.h"
#include "RigidbodyComponent.h"
#include "ServerComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthSpawnerSystem::HealthSpawnerSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::HEALTH_SPAWNER);
}

//----------------------------------------------------------------------------------------------------
bool HealthSpawnerSystem::Update()
{
    OPTICK_EVENT();

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    for (auto& entity : m_entities) {
        std::weak_ptr<HealthSpawnerComponent> healthSpawnerComponent = g_gameServer->GetComponentManager().GetComponent<HealthSpawnerComponent>(entity);
        if (!healthSpawnerComponent.expired()) {
            if (healthSpawnerComponent.lock()->m_amountSpawn == 0 || (healthSpawnerComponent.lock()->m_amountSpawn != -1 && static_cast<I32>(healthSpawnerComponent.lock()->m_amountSpawned) >= healthSpawnerComponent.lock()->m_amountSpawn)) {
                continue;
            }
            if (healthSpawnerComponent.lock()->m_entitySpawned < INVALID_ENTITY) {
                continue;
            }
            if (healthSpawnerComponent.lock()->m_timerSpawn.ReadSec() < healthSpawnerComponent.lock()->m_cooldownSpawn) {
                continue;
            }

            healthSpawnerComponent.lock()->m_entitySpawned = SpawnHealth(entity);
            ++healthSpawnerComponent.lock()->m_amountSpawned;
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void HealthSpawnerSystem::OnNotify(const Event& event)
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
Entity HealthSpawnerSystem::SpawnHealth(Entity spawner) const
{
    std::weak_ptr<HealthSpawnerComponent> healthSpawnerComponent = g_gameServer->GetComponentManager().GetComponent<HealthSpawnerComponent>(spawner);
    std::weak_ptr<TransformComponent> healthSpawnerTransformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(spawner);

    Entity entity = g_gameServer->GetEntityManager().AddEntity();
    g_gameServer->GetLinkingContext().AddEntity(entity);

    // Transform
    std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(entity);
    transformComponent.lock()->m_position = healthSpawnerTransformComponent.lock()->m_position;
    transformComponent.lock()->m_layerType = LayerType::OBJECT;
    transformComponent.lock()->m_rotation = 0.0f;
    transformComponent.lock()->m_scale = 1.0f;

    // Sprite
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(entity);
    std::weak_ptr<SpriteResource> spriteResource = g_gameServer->GetResourceManager().AddResource<SpriteResource>("objects.png", TEXTURES_DIR, true);
    spriteComponent.lock()->m_spriteResource = spriteResource;
    glm::vec4 textureCoords = glm::vec4(1.0f, 12.0f, 64.0f, 64.0f);
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

    // Health
    std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().AddComponent<HealthComponent>(entity);
    healthComponent.lock()->m_HP = healthSpawnerComponent.lock()->m_HP;
    healthComponent.lock()->m_timeHeal = healthSpawnerComponent.lock()->m_timeHeal;
    healthComponent.lock()->m_cooldownHeal = healthSpawnerComponent.lock()->m_cooldownHeal;

    return entity;
}

//----------------------------------------------------------------------------------------------------
void HealthSpawnerSystem::DespawnHealth(Entity entity) const
{
    g_gameServer->GetLinkingContext().RemoveEntity(entity);
    g_gameServer->GetEntityManager().RemoveEntity(entity);
}

//----------------------------------------------------------------------------------------------------
bool HealthSpawnerSystem::PickUpHealth(Entity character, Entity health) const
{
    std::weak_ptr<HealthComponent> healthHealthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(health);
    std::weak_ptr<BotComponent> characterBotComponent = g_gameServer->GetComponentManager().GetComponent<BotComponent>(character);
    std::weak_ptr<SpriteComponent> characterSpriteComponent = g_gameServer->GetComponentManager().GetComponent<SpriteComponent>(character);
    std::weak_ptr<HealthComponent> characterHealthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(character);

    if (healthHealthComponent.lock()->m_isPickedUp) {
        return false;
    }

    if (characterBotComponent.lock()->m_actionType == BotComponent::ActionType::HEAL) {
        return false;
    }

    U32 characterDirtyState = 0;

    if (characterBotComponent.lock()->m_actionType == BotComponent::ActionType::COOLDOWN_HEAL) {
        characterBotComponent.lock()->m_actionType = BotComponent::ActionType::NONE;
        characterDirtyState |= static_cast<U64>(ComponentMemberType::BOT_ACTION_TYPE);
    }

    healthHealthComponent.lock()->m_isPickedUp = true;

    characterHealthComponent.lock()->m_HP = healthHealthComponent.lock()->m_HP;
    characterDirtyState |= static_cast<U64>(ComponentMemberType::HEALTH_HP);
    characterHealthComponent.lock()->m_timeHeal = healthHealthComponent.lock()->m_timeHeal;
    characterDirtyState |= static_cast<U64>(ComponentMemberType::HEALTH_TIME_HEAL);
    characterHealthComponent.lock()->m_cooldownHeal = healthHealthComponent.lock()->m_cooldownHeal;
    characterDirtyState |= static_cast<U64>(ComponentMemberType::HEALTH_COOLDOWN_HEAL);

    if (characterDirtyState > 0) {
        Event newComponentEvent;
        newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
        newComponentEvent.component.entity = character;
        newComponentEvent.component.dirtyState = characterDirtyState;
        NotifyEvent(newComponentEvent);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void HealthSpawnerSystem::OnCollisionEnter(Entity entityA, Entity entityB) const
{
    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    PlayerID playerIDA = serverComponent.lock()->GetPlayerID(entityA);
    PlayerID playerIDB = serverComponent.lock()->GetPlayerID(entityB);
    if ((playerIDA < INVALID_PLAYER_ID && playerIDB < INVALID_PLAYER_ID)
        || (playerIDA >= INVALID_PLAYER_ID && playerIDB >= INVALID_PLAYER_ID)) {
        return;
    }

    if (playerIDA < INVALID_PLAYER_ID) {
        std::weak_ptr<HealthComponent> healthComponentB = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entityB);
        if (!healthComponentB.expired()) {
            const bool isPickUp = PickUpHealth(entityA, entityB);
            if (isPickUp) {
                DespawnHealth(entityB);
            }
        }
    } else if (playerIDB < INVALID_PLAYER_ID) {
        std::weak_ptr<HealthComponent> healthComponentA = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entityA);
        if (!healthComponentA.expired()) {
            const bool isPickUp = PickUpHealth(entityB, entityA);
            if (isPickUp) {
                DespawnHealth(entityA);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void HealthSpawnerSystem::OnEntityRemoved(Entity entityRemoved) const
{
    for (auto& entity : m_entities) {
        std::weak_ptr<HealthSpawnerComponent> healthSpawnerComponent = g_gameServer->GetComponentManager().GetComponent<HealthSpawnerComponent>(entity);
        if (healthSpawnerComponent.lock()->m_entitySpawned == entityRemoved) {
            healthSpawnerComponent.lock()->m_entitySpawned = INVALID_ENTITY;
            healthSpawnerComponent.lock()->m_timerSpawn.Start();
            break;
        }
    }
}
}
