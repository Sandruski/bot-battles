#include "SpawnerSystem.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "LinkingContext.h"
#include "SingletonServerComponent.h"
#include "SpriteComponent.h"
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
    transform->m_position = Vec2(225.3f, 150.3f);

    std::shared_ptr<ResourceTexture> resourceTexture = g_game->GetResourceManager().AddResource<ResourceTexture>("character.png", "../../data/textures/");
    std::shared_ptr<SpriteComponent> sprite = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(character);
    sprite->m_texture = resourceTexture;

    g_gameServer->GetComponentManager().AddComponent<InputComponent>(character);

    return character;
}
}
