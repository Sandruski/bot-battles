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
bool GameplayStateClient::Update() const
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    if (gameplayComponent.m_phase == GameplayComponent::GameplayPhase::NONE) {
        g_gameClient->GetFSM().ChangeState("Scoreboard"); // TODO: config
    }

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

    return true;
}
}
