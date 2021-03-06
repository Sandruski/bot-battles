#include "BotSpawnerSystem.h"

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
#include "MapComponent.h"
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
BotSpawnerSystem::BotSpawnerSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::BOT_SPAWNER);
}

//----------------------------------------------------------------------------------------------------
void BotSpawnerSystem::OnNotify(const Event& event)
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
Entity BotSpawnerSystem::SpawnBot(PlayerID playerID) const
{
    assert(playerID < INVALID_PLAYER_ID);

    Entity botSpawner = INVALID_ENTITY;
    for (const auto& entity : m_entities) {
        std::weak_ptr<BotSpawnerComponent> botSpawnerBotSpawnerComponent = g_gameServer->GetComponentManager().GetComponent<BotSpawnerComponent>(entity);
        if (botSpawnerBotSpawnerComponent.lock()->m_playerID == playerID) {
            botSpawner = entity;
            break;
        }
    }
    std::weak_ptr<BotSpawnerComponent> botSpawnerComponent = g_gameServer->GetComponentManager().GetComponent<BotSpawnerComponent>(botSpawner);
    std::weak_ptr<TransformComponent> botSpawnerTransformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(botSpawner);

    Entity character = g_gameServer->GetEntityManager().AddEntity();
    g_gameServer->GetLinkingContext().AddEntity(character);

    // Transform
    std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(character);
    transformComponent.lock()->m_position = botSpawnerTransformComponent.lock()->m_position;
    transformComponent.lock()->m_layerType = LayerType::PLAYER;
    transformComponent.lock()->m_rotation = botSpawnerComponent.lock()->m_transformRotation;
    std::weak_ptr<MapComponent> mapComponent = g_game->GetMapComponent();
    transformComponent.lock()->m_scale = mapComponent.lock()->m_mapScale;

    // Sprite
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(character);
    std::weak_ptr<SpriteResource> charactersSpriteResource = g_gameServer->GetResourceManager().AddResource<SpriteResource>("characters.png", TEXTURES_DIR, true);
    spriteComponent.lock()->m_spriteResource = charactersSpriteResource;
    U32 playerNumber = playerID + 1;
    switch (playerNumber) {
    case 1: {
        glm::vec4 idleTextureCoords = glm::vec4(1.0f, 1.0f, 36.0f, 43.0f);
        spriteComponent.lock()->AddSprite("idle", idleTextureCoords);
        glm::vec4 healTextureCoords = glm::vec4(38.0f, 1.0f, 38.0f, 43.0f);
        spriteComponent.lock()->AddSprite("heal", healTextureCoords);
        glm::vec4 shootSecondaryTextureCoords = glm::vec4(77.0f, 1.0f, 52.0f, 43.0f);
        spriteComponent.lock()->AddSprite("shootSecondary", shootSecondaryTextureCoords);
        glm::vec4 shootPrimaryTextureCoords = glm::vec4(130.0f, 1.0f, 52.0f, 43.0f);
        spriteComponent.lock()->AddSprite("shootPrimary", shootPrimaryTextureCoords);
        glm::vec4 reloadTextureCoords = glm::vec4(183.0f, 1.0f, 42.0f, 43.0f);
        spriteComponent.lock()->AddSprite("reload", reloadTextureCoords);
        glm::vec4 dieTextureCoords = glm::vec4(1.0f, 89.0f, 64.0f, 63.0f);
        spriteComponent.lock()->AddSprite("die", dieTextureCoords);
        break;
    }
    case 2: {
        glm::vec4 idleTextureCoords = glm::vec4(1.0f, 45.0f, 35.0f, 43.0f);
        spriteComponent.lock()->AddSprite("idle", idleTextureCoords);
        glm::vec4 healTextureCoords = glm::vec4(38.0f, 45.0f, 37.0f, 43.0f);
        spriteComponent.lock()->AddSprite("heal", healTextureCoords);
        glm::vec4 shootSecondaryTextureCoords = glm::vec4(77.0f, 45.0f, 51.0f, 43.0f);
        spriteComponent.lock()->AddSprite("shootSecondary", shootSecondaryTextureCoords);
        glm::vec4 shootPrimaryTextureCoords = glm::vec4(130.0f, 45.0f, 51.0f, 43.0f);
        spriteComponent.lock()->AddSprite("shootPrimary", shootPrimaryTextureCoords);
        glm::vec4 reloadTextureCoords = glm::vec4(183.0f, 45.0f, 41.0f, 43.0f);
        spriteComponent.lock()->AddSprite("reload", reloadTextureCoords);
        glm::vec4 dieTextureCoords = glm::vec4(1.0f, 89.0f, 64.0f, 63.0f);
        spriteComponent.lock()->AddSprite("die", dieTextureCoords);
        break;
    }
    default: {
        break;
    }
    }
    spriteComponent.lock()->m_spriteName = "idle";

    // Label
    std::weak_ptr<LabelComponent> labelComponent = g_gameServer->GetComponentManager().AddComponent<LabelComponent>(character);
    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
    labelComponent.lock()->m_text = clientProxy.lock()->GetName();
    labelComponent.lock()->m_text.append(" (P");
    labelComponent.lock()->m_text.append(std::to_string(playerNumber));
    labelComponent.lock()->m_text.append(")");
    labelComponent.lock()->m_offset = glm::vec2(0.0f, 35.0f);
    switch (playerID) {
    case 0: {
        labelComponent.lock()->m_color = Red;
        break;
    }
    case 1: {
        labelComponent.lock()->m_color = Blue;
        break;
    }
    default: {
        break;
    }
    }

    // Collider
    std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().AddComponent<ColliderComponent>(character);
    colliderComponent.lock()->m_size = glm::vec2(30.0f, 30.0f);
    colliderComponent.lock()->m_size *= transformComponent.lock()->m_scale;
    colliderComponent.lock()->m_shapeType = ColliderComponent::ShapeType::CIRCLE;

    // Rigidbody
    std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().AddComponent<RigidbodyComponent>(character);
    rigidbodyComponent.lock()->m_maxLinearVelocity = botSpawnerComponent.lock()->m_rigidbodyMaxLinearVelocity;
    rigidbodyComponent.lock()->m_maxAngularVelocity = botSpawnerComponent.lock()->m_rigidbodyMaxAngularVelocity;
    rigidbodyComponent.lock()->m_bodyType = RigidbodyComponent::BodyType::DYNAMIC;
    rigidbodyComponent.lock()->UpdateBodyType();
    rigidbodyComponent.lock()->m_groupIndex = -1;
    rigidbodyComponent.lock()->UpdateGroupIndex();
    rigidbodyComponent.lock()->m_isBullet = true;
    rigidbodyComponent.lock()->UpdateBullet();

    // Weapon
    std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().AddComponent<WeaponComponent>(character);
    weaponComponent.lock()->m_damageSecondary = botSpawnerComponent.lock()->m_weaponDamage;
    weaponComponent.lock()->m_rangeSecondary = botSpawnerComponent.lock()->m_weaponRange;
    weaponComponent.lock()->m_timeShootSecondary = botSpawnerComponent.lock()->m_weaponTimeShoot;
    weaponComponent.lock()->m_cooldownShootSecondary = botSpawnerComponent.lock()->m_weaponCooldownShoot;

    // Health
    std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().AddComponent<HealthComponent>(character);
    healthComponent.lock()->m_maxHP = botSpawnerComponent.lock()->m_healthMaxHP;
    healthComponent.lock()->m_currentHP = healthComponent.lock()->m_maxHP;

    // Sight
    std::weak_ptr<SightComponent> sightComponent = g_gameServer->GetComponentManager().AddComponent<SightComponent>(character);
    sightComponent.lock()->m_angle = botSpawnerComponent.lock()->m_sightAngle;
    sightComponent.lock()->m_distance = botSpawnerComponent.lock()->m_sightDistance;

    // Bot
    g_gameServer->GetComponentManager().AddComponent<BotComponent>(character);

    // Player
    g_gameServer->GetComponentManager().AddComponent<PlayerComponent>(character);

    return character;
}

//----------------------------------------------------------------------------------------------------
void BotSpawnerSystem::DespawnBot(Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    g_gameServer->GetLinkingContext().RemoveEntity(entity);
    g_gameServer->GetEntityManager().RemoveEntity(entity);
}

//----------------------------------------------------------------------------------------------------
void BotSpawnerSystem::OnPlayerAdded(PlayerID playerID) const
{
    assert(playerID < INVALID_PLAYER_ID);

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    Entity entity = serverComponent.lock()->GetEntity(playerID);
    if (entity < INVALID_ENTITY) {
        WLOG("Entity not created because player %u already has an entity", playerID);
        return;
    }

    entity = SpawnBot(playerID);
    serverComponent.lock()->AddEntity(entity, playerID);
}

//----------------------------------------------------------------------------------------------------
void BotSpawnerSystem::OnPlayerRemoved(Entity entity) const
{
    if (entity >= INVALID_ENTITY) {
        return;
    }

    DespawnBot(entity);
    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    serverComponent.lock()->RemoveEntity(entity);
}
}
