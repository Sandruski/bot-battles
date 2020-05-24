#include "SpawnerSystem.h"

#include "BotComponent.h"
#include "BotSpawnerComponent.h"
#include "ClientProxy.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "HealthComponent.h"
#include "LabelComponent.h"
#include "LinkingContext.h"
#include "PlayerComponent.h"
#include "RendererComponent.h"
#include "ResourceManager.h"
#include "RigidbodyComponent.h"
#include "ServerComponent.h"
#include "SightComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SpawnerSystem::SpawnerSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::BOT_SPAWNER);
}

//----------------------------------------------------------------------------------------------------
void SpawnerSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::PLAYER_ADDED: {
        OnPlayerAdded(event.networking.playerID);
        break;
    }

    case EventType::PLAYER_REMOVED: {
        OnPlayerRemoved(event.networking.entity);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
Entity SpawnerSystem::Spawn(PlayerID playerID) const
{
    Entity character = g_gameServer->GetEntityManager().AddEntity();
    g_gameServer->GetLinkingContext().AddEntity(character);

    std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(character);
    for (const auto& entity : m_entities) {
        std::weak_ptr<BotSpawnerComponent> botSpawnerComponent = g_gameServer->GetComponentManager().GetComponent<BotSpawnerComponent>(entity);
        if (botSpawnerComponent.lock()->m_playerID == playerID) {
            std::weak_ptr<TransformComponent> botSpawnerTransformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
            transformComponent.lock()->m_position = botSpawnerTransformComponent.lock()->m_position;
            transformComponent.lock()->m_layerType = LayerType::PLAYER;
            break;
        }
    }

    std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(character);
    U32 playerNumber = playerID + 1;
    switch (playerNumber) {
    case 1: {
        std::weak_ptr<SpriteResource> charactersSpriteResource = g_game->GetResourceManager().AddResource<SpriteResource>("greenCharacter.png", TEXTURES_DIR, true);
        spriteComponent.lock()->m_spriteResource = charactersSpriteResource;

        glm::vec4 standTextureCoords = glm::vec4(1.0f, 1.0f, 36.0f, 43.0f);
        spriteComponent.lock()->AddSprite("stand", standTextureCoords);
        glm::vec4 holdTextureCoords = glm::vec4(38.0f, 1.0f, 38.0f, 43.0f);
        spriteComponent.lock()->AddSprite("hold", holdTextureCoords);
        glm::vec4 shootTextureCoords = glm::vec4(77.0f, 1.0f, 52.0f, 43.0f);
        spriteComponent.lock()->AddSprite("shoot", shootTextureCoords);
        glm::vec4 realoadTextureCoords = glm::vec4(130.0f, 1.0f, 42.0f, 43.0f);
        spriteComponent.lock()->AddSprite("reaload", realoadTextureCoords);
        spriteComponent.lock()->m_spriteName = "stand";
        break;
    }
    case 2: {
        std::weak_ptr<SpriteResource> charactersSpriteResource = g_game->GetResourceManager().AddResource<SpriteResource>("blueCharacter.png", TEXTURES_DIR, true);
        spriteComponent.lock()->m_spriteResource = charactersSpriteResource;

        glm::vec4 standTextureCoords = glm::vec4(1.0f, 1.0f, 36.0f, 43.0f);
        spriteComponent.lock()->AddSprite("stand", standTextureCoords);
        glm::vec4 holdTextureCoords = glm::vec4(38.0f, 1.0f, 38.0f, 43.0f);
        spriteComponent.lock()->AddSprite("hold", holdTextureCoords);
        glm::vec4 shootTextureCoords = glm::vec4(77.0f, 1.0f, 52.0f, 43.0f);
        spriteComponent.lock()->AddSprite("shoot", shootTextureCoords);
        glm::vec4 realoadTextureCoords = glm::vec4(130.0f, 1.0f, 42.0f, 43.0f);
        spriteComponent.lock()->AddSprite("reaload", realoadTextureCoords);
        spriteComponent.lock()->m_spriteName = "stand";
        break;
    }
    default: {
        break;
    }
    }

    std::weak_ptr<LabelComponent> labelComponent = g_gameServer->GetComponentManager().AddComponent<LabelComponent>(character);
    std::string playerName = "Player ";
    playerName.append(std::to_string(playerNumber));
    labelComponent.lock()->m_text = playerName;
    labelComponent.lock()->m_offset = glm::vec2(0.0f, -35.0f);
    switch (playerNumber) {
    case 1: {
        labelComponent.lock()->m_color = Red;
        break;
    }
    case 2: {
        labelComponent.lock()->m_color = Blue;
        break;
    }
    default: {
        break;
    }
    }

    std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().AddComponent<ColliderComponent>(character);
    const glm::uvec4 spriteTextureCoords = spriteComponent.lock()->GetSpriteTextureCoords();
    colliderComponent.lock()->m_size = glm::vec2(30.0f, 30.0f);
    colliderComponent.lock()->m_shapeType = ColliderComponent::ShapeType::CIRCLE;

    std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().AddComponent<RigidbodyComponent>(character);
    rigidbodyComponent.lock()->m_bodyType = RigidbodyComponent::BodyType::DYNAMIC;
    rigidbodyComponent.lock()->UpdateBodyType();
    rigidbodyComponent.lock()->m_groupIndex = 1;
    rigidbodyComponent.lock()->UpdateGroupIndex();
    rigidbodyComponent.lock()->m_isBullet = true;
    rigidbodyComponent.lock()->UpdateBullet();

    std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().AddComponent<WeaponComponent>(character);
    weaponComponent.lock()->m_damage = 20;

    std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().AddComponent<HealthComponent>(character);
    healthComponent.lock()->m_health = 100;

    std::weak_ptr<SightComponent> sightComponent = g_gameServer->GetComponentManager().AddComponent<SightComponent>(character);
    sightComponent.lock()->m_angle = 90.0f;
    sightComponent.lock()->m_distance = 250.0f;

    g_gameServer->GetComponentManager().AddComponent<BotComponent>(character);

    g_gameServer->GetComponentManager().AddComponent<PlayerComponent>(character);

    return character;
}

//----------------------------------------------------------------------------------------------------
void SpawnerSystem::Despawn(Entity entity) const
{
    g_gameServer->GetLinkingContext().RemoveEntity(entity);
    g_gameServer->GetEntityManager().RemoveEntity(entity);
}

//----------------------------------------------------------------------------------------------------
void SpawnerSystem::OnPlayerAdded(PlayerID playerID) const
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    Entity entity = serverComponent.GetEntity(playerID);
    if (entity < INVALID_ENTITY) {
        WLOG("Entity not created because player %u already has an entity", playerID);
        return;
    }

    entity = Spawn(playerID);
    serverComponent.AddEntity(entity, playerID);
}

//----------------------------------------------------------------------------------------------------
void SpawnerSystem::OnPlayerRemoved(Entity entity) const
{
    Despawn(entity);
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    serverComponent.RemoveEntity(entity);
}
}
