#include "GameplayStateClient.h"

#include "Config.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameClient.h"
#include "GameComponent.h"
#include "LinkingContext.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
const char* GameplayStateClient::GetName() const
{
    return "Gameplay";
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::Enter() const
{
    ILOG("Entering GameplayStateClient...");

    GameComponent& gameComponent = g_gameClient->GetGameComponent();
    gameComponent.m_phaseType = PhaseType::WAIT;

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    std::string path;
    path.append(MAPS_DIR);
    path.append(clientComponent.m_map);
    g_gameClient->GetMapImporter().Load(path);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::Exit() const
{
    ILOG("Exiting GameplayStateClient...");

    g_gameClient->GetLinkingContext().ClearEntities();
    g_gameClient->GetEntityManager().ClearEntities();

    return true;
}

//----------------------------------------------------------------------------------------------------
void GameplayStateClient::OnNotify(const Event& event)
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
void GameplayStateClient::OnPlayerAdded() const
{
    // TODO: receive events for remote players
}

//----------------------------------------------------------------------------------------------------
void GameplayStateClient::OnPlayerRemoved() const
{
    g_gameClient->GetFSM().ChangeState(g_gameClient->GetConfig().m_offlineSceneName.c_str());
}
}
