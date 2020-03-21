#include "SpawnerSystem.h"

#include "ClientProxy.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "HealthComponent.h"
#include "LinkingContext.h"
#include "RendererComponent.h"
#include "ResourceManager.h"
#include "ServerComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WeaponComponent.h"

namespace sand {

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
Entity SpawnerSystem::Spawn(U32 number) const
{
    Entity character = g_gameServer->GetEntityManager().AddEntity();
    g_gameServer->GetLinkingContext().AddEntity(character);

    std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(character);
    transformComponent.lock()->m_position = glm::vec3(225.0f, 150.0f, 1.0f);

    std::weak_ptr<SpriteResource> charactersSpriteResource = g_game->GetResourceManager().AddResource<SpriteResource>("characters.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(character);
    spriteComponent.lock()->m_spriteResource = charactersSpriteResource;
    switch (number) {
    case 1: {
        glm::vec4 standTextureCoords = glm::vec4(1.0f, 1.0f, 36.0f, 43.0f);
        spriteComponent.lock()->AddSprite("stand", standTextureCoords);
        glm::vec4 holdTextureCoords = glm::vec4(38.0f, 1.0f, 38.0f, 43.0f);
        spriteComponent.lock()->AddSprite("hold", holdTextureCoords);
        glm::vec4 shootTextureCoords = glm::vec4(77.0f, 1.0f, 52.0f, 43.0f);
        spriteComponent.lock()->AddSprite("shoot", shootTextureCoords);
        glm::vec4 realoadTextureCoords = glm::vec4(130.0f, 1.0f, 42.0f, 43.0f);
        spriteComponent.lock()->AddSprite("reaload", realoadTextureCoords);
        spriteComponent.lock()->m_currentSprite = "stand";
        break;
    }

    case 2: {
        glm::vec4 standTextureCoords = glm::vec4(1.0f, 45.0f, 35.0f, 43.0f);
        spriteComponent.lock()->AddSprite("stand", standTextureCoords);
        glm::vec4 holdTextureCoords = glm::vec4(37.0f, 45.0f, 37.0f, 43.0f);
        spriteComponent.lock()->AddSprite("hold", holdTextureCoords);
        glm::vec4 shootTextureCoords = glm::vec4(75.0f, 45.0f, 51.0f, 43.0f);
        spriteComponent.lock()->AddSprite("shoot", shootTextureCoords);
        glm::vec4 realoadTextureCoords = glm::vec4(127.0f, 45.0f, 42.0f, 43.0f);
        spriteComponent.lock()->AddSprite("reaload", realoadTextureCoords);
        spriteComponent.lock()->m_currentSprite = "stand";
        break;
    }

    default: {
        break;
    }
    }

    std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().AddComponent<ColliderComponent>(character);
    //const SDL_Rect& currentSprite = spriteComponent.lock()->GetCurrentSprite(); // TODO
    //colliderComponent.lock()->m_size.x = static_cast<F32>(currentSprite.w);
    //colliderComponent.lock()->m_size.y = static_cast<F32>(currentSprite.h);

    std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().AddComponent<WeaponComponent>(character);

    std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().AddComponent<HealthComponent>(character);
    healthComponent.lock()->m_health = 100;

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
    U32 playerNumber = playerID + 1;
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
    Entity entity = Spawn(playerNumber);
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
