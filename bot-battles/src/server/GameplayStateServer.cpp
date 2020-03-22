#include "GameplayStateServer.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameComponent.h"
#include "GameServer.h"
#include "LinkingContext.h"
#include "MapImporter.h"
#include "ResourceManager.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
const char* GameplayStateServer::GetName() const
{
    return "Gameplay";
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::Enter() const
{
    ILOG("Entering GameplayStateServer...");

    GameComponent& gameComponent = g_gameServer->GetGameComponent();
    gameComponent.m_phaseType = PhaseType::WAIT;

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    std::string path;
    path.append(MAPS_DIR);
    path.append(serverComponent.m_map);
    g_gameServer->GetMapImporter().Load(path);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::Exit() const
{
    ILOG("Exiting GameplayStateServer...");

    g_game->GetLinkingContext().ClearEntities();
    g_game->GetEntityManager().ClearEntities();

    return true;
}

//----------------------------------------------------------------------------------------------------
void GameplayStateServer::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::PLAYER_ADDED: {
        OnPlayerAdded();
        break;
    }

    case EventType::PLAYER_REMOVED: {
        OnPlayerRemoved();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void GameplayStateServer::OnPlayerAdded() const
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    U32 playerCount = serverComponent.GetPlayerCount();
    if (playerCount == MAX_PLAYER_IDS) {
        GameComponent& gameComponent = g_gameServer->GetGameComponent();
        gameComponent.m_phaseType = PhaseType::PLAY;
    }
}

//----------------------------------------------------------------------------------------------------
void GameplayStateServer::OnPlayerRemoved() const
{
    GameComponent& gameComponent = g_gameServer->GetGameComponent();
    if (gameComponent.m_phaseType == PhaseType::PLAY) {
        gameComponent.m_phaseType = PhaseType::RESTART;
    }
}
}
