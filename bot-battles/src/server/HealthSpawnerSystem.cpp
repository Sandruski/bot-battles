#include "HealthSpawnerSystem.h"

#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "HealthComponent.h"
#include "HealthSpawnerComponent.h"
#include "LinkingContext.h"
#include "RigidbodyComponent.h"
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
    // TODO: spawn only once or forever
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    for (auto& entity : m_entities) {
        std::weak_ptr<HealthSpawnerComponent> healthSpawnerComponent = g_gameServer->GetComponentManager().GetComponent<HealthSpawnerComponent>(entity);
        if (!healthSpawnerComponent.expired()) {
            if (!healthSpawnerComponent.lock()->m_spawnHealth1 && !healthSpawnerComponent.lock()->m_spawnHealth2) {
                continue;
            }
            if (healthSpawnerComponent.lock()->m_entitySpawned < INVALID_ENTITY) {
                continue;
            }
            if (healthSpawnerComponent.lock()->m_timerSpawn.ReadSec() < healthSpawnerComponent.lock()->m_timeoutSpawn) {
                continue;
            }
            healthSpawnerComponent.lock()->m_timerSpawn.Start();

            U32 health1 = healthSpawnerComponent.lock()->m_spawnHealth1 ? 1 : 2;
            U32 health2 = healthSpawnerComponent.lock()->m_spawnHealth2 ? 2 : 1;
            U32 health = RandomInt(health1, health2);
            healthSpawnerComponent.lock()->m_entitySpawned = SpawnHealth(health, entity);
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void HealthSpawnerSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {

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
Entity HealthSpawnerSystem::SpawnHealth(U32 health, Entity spawner) const
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
    switch (health) {
    case 1: {
        glm::vec4 textureCoords = glm::vec4(1.0f, 12.0f, 64.0f, 64.0f);
        spriteComponent.lock()->AddSprite("default", textureCoords);
        break;
    }
    case 2: {
        glm::vec4 textureCoords = glm::vec4(66.0f, 12.0f, 64.0f, 64.0f);
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

    // Health
    std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().AddComponent<HealthComponent>(entity);
    switch (health) {
    case 1: {
        healthComponent.lock()->m_currentHealth = healthComponent.lock()->m_maxHealth = healthSpawnerComponent.lock()->m_pointsHealth1;
        break;
    }
    case 2: {
        healthComponent.lock()->m_currentHealth = healthComponent.lock()->m_maxHealth = healthSpawnerComponent.lock()->m_pointsHealth2;
        break;
    }
    default: {
        break;
    }
    }

    return entity;
}

//----------------------------------------------------------------------------------------------------
void HealthSpawnerSystem::OnEntityRemoved(Entity entityRemoved) const
{
    for (auto& entity : m_entities) {
        std::weak_ptr<HealthSpawnerComponent> healthSpawnerComponent = g_gameServer->GetComponentManager().GetComponent<HealthSpawnerComponent>(entity);
        if (!healthSpawnerComponent.expired()) {
            if (healthSpawnerComponent.lock()->m_entitySpawned == entityRemoved) {
                healthSpawnerComponent.lock()->m_entitySpawned = INVALID_ENTITY;
                break;
            }
        }
    }
}
}
