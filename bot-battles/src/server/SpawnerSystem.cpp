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
Entity SpawnerSystem::SpawnPlayerEntity() const
{
    Entity character = g_gameServer->GetEntityManager().AddEntity();
    g_gameServer->GetLinkingContext().AddEntity(character);

    std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(character);
    transformComponent.lock()->m_position = Vec3(225.3f, 150.3f, 1.0f);

    std::weak_ptr<SpriteResource> spriteResource = g_game->GetResourceManager().AddResource<SpriteResource>("character.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(character);
    spriteComponent.lock()->m_sprite = spriteResource;

    std::weak_ptr<TextResource> textResource = g_game->GetResourceManager().AddResource<TextResource>("", "", false);
    RendererComponent& rendererComponent = g_game->GetRendererComponent();
    textResource.lock()->m_font = rendererComponent.m_defaultFont;
    textResource.lock()->m_text = spriteResource.lock()->GetFile();
    textResource.lock()->m_color = Red;
    textResource.lock()->ReLoad();
    std::weak_ptr<TextComponent> textComponent = g_gameServer->GetComponentManager().AddComponent<TextComponent>(character);
    textComponent.lock()->m_text = textResource;

    return character;
}

//----------------------------------------------------------------------------------------------------
void SpawnerSystem::OnPlayerAdded(PlayerID playerID) const
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    Entity entity = SpawnPlayerEntity();
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
