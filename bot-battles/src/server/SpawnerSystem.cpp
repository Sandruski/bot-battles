#include "SpawnerSystem.h"

#include "ClientProxy.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "LinkingContext.h"
#include "RendererComponent.h"
#include "ResourceManager.h"
#include "ServerComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TextComponent.h"
#include "TextResource.h"
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
Entity SpawnerSystem::SpawnPlayerEntity(U32 number, const std::string& name) const
{
    Entity character = g_gameServer->GetEntityManager().AddEntity();
    g_gameServer->GetLinkingContext().AddEntity(character);

    std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(character);
    transformComponent.lock()->m_position = Vec3(225.0f, 150.0f, 1.0f);

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

    std::weak_ptr<TextResource> textResource = g_game->GetResourceManager().AddResource<TextResource>("", "", false);
    RendererComponent& rendererComponent = g_game->GetRendererComponent();
    textResource.lock()->m_font = rendererComponent.m_defaultFont;
    textResource.lock()->m_text = name;
    textResource.lock()->m_color = Black;
    textResource.lock()->ReLoad();
    std::weak_ptr<TextComponent> textComponent = g_gameServer->GetComponentManager().AddComponent<TextComponent>(character);
    textComponent.lock()->m_text = textResource;

    std::weak_ptr<ColliderComponent> colliderComponent = g_gameServer->GetComponentManager().AddComponent<ColliderComponent>(character);
    const SDL_Rect& currentSprite = spriteComponent.lock()->GetCurrentSprite();
    colliderComponent.lock()->m_size.x = static_cast<F32>(currentSprite.w);
    colliderComponent.lock()->m_size.y = static_cast<F32>(currentSprite.h);
    std::weak_ptr<WeaponComponent> weaponComponent = g_gameServer->GetComponentManager().AddComponent<WeaponComponent>(character);

    return character;
}

//----------------------------------------------------------------------------------------------------
void SpawnerSystem::OnPlayerAdded(PlayerID playerID) const
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
    U32 playerNumber = playerID + 1;
    const std::string playerName = clientProxy.lock()->GetName();
    Entity entity = SpawnPlayerEntity(playerNumber, playerName);
    serverComponent.AddEntity(entity, playerID);
}

//----------------------------------------------------------------------------------------------------
void SpawnerSystem::OnPlayerRemoved(Entity entity) const
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    g_gameServer->GetLinkingContext().RemoveEntity(entity);
    g_game->GetEntityManager().RemoveEntity(entity);
    serverComponent.RemoveEntity(entity);
}
}
