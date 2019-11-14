#include "SpawnerSystem.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "Game.h"
#include "LinkingContext.h"
#include "SingletonServerComponent.h"
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
        std::shared_ptr<SingletonServerComponent> server = g_game->GetSingletonServerComponent();
        std::shared_ptr<ClientProxy> clientProxy = server->GetClientProxy(event.server.playerID);
        clientProxy->m_entity = SpawnPlayerEntity();
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
    Entity character = g_game->GetEntityManager().AddEntity();
    std::shared_ptr<TransformComponent> transform = g_game->GetComponentManager().AddComponent<TransformComponent>(character);
    transform->m_position = Vec2(10.0f, 40.0f);

    g_game->GetLinkingContext().AddEntity(character);

    return character;
}
}
