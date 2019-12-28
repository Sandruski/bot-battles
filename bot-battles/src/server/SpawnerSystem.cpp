#include "SpawnerSystem.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "LinkingContext.h"
#include "RendererComponent.h"
#include "ResourceManager.h"
#include "ServerComponent.h"
#include "SpriteComponent.h"
#include "TextComponent.h"
#include "TextResource.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
SpawnerSystem::SpawnerSystem()
{
}

//----------------------------------------------------------------------------------------------------
SpawnerSystem::~SpawnerSystem()
{
}

//----------------------------------------------------------------------------------------------------
void SpawnerSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {
    case EventType::PLAYER_ADDED: {
        ServerComponent& serverComponent = g_gameServer->GetServerComponent();
        Entity entity = SpawnPlayerEntity();
        serverComponent.AddEntity(entity, event.server.playerID);
        break;
    }

    case EventType::PLAYER_REMOVED: {
        ServerComponent& serverComponent = g_gameServer->GetServerComponent();
        g_game->GetEntityManager().RemoveEntity(event.server.entity);
        serverComponent.RemoveEntity(event.server.entity);
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

    std::weak_ptr<TransformComponent> transform = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(character);
    static float zeta = -1.0f;
    transform.lock()->m_position = Vec3(225.3f, 150.3f, zeta++);

    std::shared_ptr<SpriteResource> spriteResource = g_game->GetResourceManager().AddResource<SpriteResource>("character.png", "../../data/textures/", true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(character);
    spriteComponent.lock()->m_sprite = spriteResource;

    std::shared_ptr<TextResource> textResource = g_game->GetResourceManager().AddResource<TextResource>("", "", false);
    RendererComponent& rendererComponent = g_game->GetRendererComponent();
    textResource->SetFont(rendererComponent.m_font);
    textResource->SetText(spriteResource->GetFile());
    textResource->SetColor(Red);
    textResource->ReLoad();
    std::weak_ptr<TextComponent> textComponent = g_gameServer->GetComponentManager().AddComponent<TextComponent>(character);
    textComponent.lock()->m_text = textResource;

    g_gameServer->GetComponentManager().AddComponent<InputComponent>(character);

    return character;
}
}
