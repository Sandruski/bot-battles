#include "GameplayStateClient.h"

#include "EntityManager.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "GuiComponent.h"
#include "LinkingContext.h"
#include "MapComponent.h"
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
    bool ret = false;

    // Scene
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    std::string path = MAPS_DIR;
    path.append(clientComponent.m_map);
    path.append(MAPS_EXTENSION);
    MapImporter::Tilemap tilemap;
    ret = g_gameClient->GetMapImporter().Load(path, tilemap);
    if (!ret) {
        return ret;
    }
    g_gameClient->GetMapImporter().Create(tilemap);

    GuiComponent& guiComponent = g_gameClient->GetGuiComponent();
    guiComponent.m_isSettings = false;

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    gameplayComponent.m_mainMenuTimer.Start();
    ret = gameplayComponent.m_fsm.ChangeState("Start");

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::Update() const
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    return gameplayComponent.m_fsm.Update();
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::RenderGui() const
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    return gameplayComponent.m_fsm.RenderGui();
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

    InputComponent& inputComponent = g_gameClient->GetInputComponent();
    inputComponent.FullReset();

    MapComponent& mapComponent = g_game->GetMapComponent();
    mapComponent.Reset();

    return true;
}

//----------------------------------------------------------------------------------------------------
void GameplayStateClient::OnNotify(const Event& event)
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    gameplayComponent.m_fsm.OnNotify(event);
}
}
