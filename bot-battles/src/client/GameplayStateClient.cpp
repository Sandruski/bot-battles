#include "GameplayStateClient.h"

#include "Config.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "LinkingContext.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
const char* GameplayStateClient::GetName() const
{
    return "Gameplay";
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::Enter() const
{
    ILOG("Entering %s...", GetName());

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    std::string path;
    path.append(MAPS_DIR);
    path.append(clientComponent.m_map);
    MapImporter::Tilemap tilemap = g_gameClient->GetMapImporter().Load(path);
    g_gameClient->GetMapImporter().Create(tilemap);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::Exit() const
{
    ILOG("Exiting %s...", GetName());

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    gameplayComponent.m_phase = GameplayComponent::GameplayPhase::NONE;

    g_gameClient->GetLinkingContext().ClearEntities();
    g_gameClient->GetEntityManager().ClearEntities();

    // TODO: do it through the event PLAYER_REMOVED
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    clientComponent.m_entity = INVALID_ENTITY;
    clientComponent.Reset();

    return true;
}

//----------------------------------------------------------------------------------------------------
void GameplayStateClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::RESULT_RECEIVED: {
        OnResultReceived();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void GameplayStateClient::OnResultReceived() const
{
    g_gameClient->GetFSM().ChangeState("Scoreboard"); // TODO: config
}
}
