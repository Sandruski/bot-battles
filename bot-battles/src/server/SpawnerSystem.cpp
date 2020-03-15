#include "SpawnerSystem.h"

#include "ClientProxy.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "HealthComponent.h"
#include "LinkingContext.h"
#include "MeshComponent.h"
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
        SDL_Rect standRect = { 1, 1, 36, 43 };
        SDL_Rect holdRect = { 38, 1, 38, 43 };
        SDL_Rect shootRect = { 77, 1, 52, 43 };
        SDL_Rect reloadRect = { 130, 1, 42, 43 };
        spriteComponent.lock()->AddSprite("stand", standRect);
        spriteComponent.lock()->AddSprite("hold", holdRect);
        spriteComponent.lock()->AddSprite("shoot", shootRect);
        spriteComponent.lock()->AddSprite("reload", reloadRect);
        spriteComponent.lock()->m_currentSprite = "stand";
        break;
    }

    case 2: {
        SDL_Rect standRect = { 1, 45, 35, 43 };
        SDL_Rect holdRect = { 37, 45, 37, 43 };
        SDL_Rect shootRect = { 75, 45, 51, 43 };
        SDL_Rect reloadRect = { 127, 45, 41, 43 };
        spriteComponent.lock()->AddSprite("stand", standRect);
        spriteComponent.lock()->AddSprite("hold", holdRect);
        spriteComponent.lock()->AddSprite("shoot", shootRect);
        spriteComponent.lock()->AddSprite("reload", reloadRect);
        spriteComponent.lock()->m_currentSprite = "stand";
        break;
    }

    default: {
        break;
    }
    }

    std::weak_ptr<MeshComponent> meshComponent = g_gameServer->GetComponentManager().AddComponent<MeshComponent>(character);
    meshComponent.lock()->Init();

    std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().AddComponent<ColliderComponent>(character);
    const SDL_Rect& currentSprite = spriteComponent.lock()->GetCurrentSprite();
    colliderComponent.lock()->m_size.x = static_cast<F32>(currentSprite.w);
    colliderComponent.lock()->m_size.y = static_cast<F32>(currentSprite.h);

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
