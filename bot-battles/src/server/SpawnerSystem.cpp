#include "SpawnerSystem.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "LinkingContext.h"
#include "ResourceManager.h"
#include "SingletonRendererComponent.h"
#include "SingletonServerComponent.h"
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
        std::shared_ptr<SingletonServerComponent> singletonServer = g_gameServer->GetSingletonServerComponent();
        Entity entity = SpawnPlayerEntity();
        singletonServer->AddEntity(entity, event.server.playerID);
        break;
    }

    case EventType::PLAYER_REMOVED: {
        std::shared_ptr<SingletonServerComponent> singletonServer = g_gameServer->GetSingletonServerComponent();
        g_game->GetEntityManager().RemoveEntity(event.server.entity);
        singletonServer->RemoveEntity(event.server.entity);
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

    std::shared_ptr<TransformComponent> transform = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(character);
    static float zeta = -1.0f;
    transform->m_position = Vec3(225.3f, 150.3f, zeta++);

    std::shared_ptr<SpriteResource> spriteResource = g_game->GetResourceManager().AddResource<SpriteResource>("character.png", "../../data/textures/", true);
    std::shared_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(character);
    spriteComponent->m_sprite = spriteResource;

    std::shared_ptr<TextResource> textResource = g_game->GetResourceManager().AddResource<TextResource>("", "", false);
    std::shared_ptr<SingletonRendererComponent> singletonRenderer = g_game->GetSingletonRendererComponent();
    textResource->SetFont(singletonRenderer->m_font);
    textResource->SetText(spriteResource->GetFile());
    textResource->SetColor(Red);
    textResource->ReLoad();
    std::shared_ptr<TextComponent> textComponent = g_gameServer->GetComponentManager().AddComponent<TextComponent>(character);
    textComponent->m_text = textResource;

    g_gameServer->GetComponentManager().AddComponent<InputComponent>(character);

    return character;
}
}
