#include "GameplayStateServer.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "GameplayComponent.h"
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

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    gameplayComponent.m_phase = GameplayComponent::GameplayPhase::START;

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

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    gameplayComponent.m_phase = GameplayComponent::GameplayPhase::NONE;

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
        GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
        gameplayComponent.m_phase = GameplayComponent::GameplayPhase::PLAY;
    }
}

//----------------------------------------------------------------------------------------------------
void GameplayStateServer::OnPlayerRemoved() const
{
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    if (gameplayComponent.m_phase == GameplayComponent::GameplayPhase::PLAY) {
        gameplayComponent.m_phase = GameplayComponent::GameplayPhase::START;
    }
}
}
