#include "GameplayStateClient.h"

#include "EntityManager.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "GuiComponent.h"
#include "LinkingContext.h"
#include "PlayStateClient.h"
#include "StartStateClient.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string GameplayStateClient::GetName() const
{
    return "Gameplay";
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::Create() const
{
    bool ret = false;

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    ret = gameplayComponent.m_fsm.RegisterState<StartStateClient>();
    if (!ret) {
        return ret;
    }
    ret = gameplayComponent.m_fsm.RegisterState<PlayStateClient>();
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::Enter() const
{
    // Scene
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    std::string path;
    path.append(MAPS_DIR);
    path.append(clientComponent.m_map);
    MapImporter::Tilemap tilemap = g_gameClient->GetMapImporter().Load(path);
    g_gameClient->GetMapImporter().Create(tilemap);

    GuiComponent& guiComponent = g_gameClient->GetGuiComponent();
    guiComponent.m_isSettings = false;

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    return gameplayComponent.m_fsm.ChangeState("Start");
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::Exit() const
{
    // Scene
    g_gameClient->GetLinkingContext().ClearEntities();
    g_gameClient->GetEntityManager().ClearEntities();

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    clientComponent.Reset();

    clientComponent.m_entity = INVALID_ENTITY;

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> emptyState = std::weak_ptr<State>();
    gameplayComponent.m_fsm.ChangeState(emptyState);

    return true;
}

//----------------------------------------------------------------------------------------------------
void GameplayStateClient::OnNotify(const Event& event)
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    gameplayComponent.m_fsm.OnNotify(event);
}
}
