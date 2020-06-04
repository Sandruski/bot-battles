#include "GameplayStateClient.h"

#include "ClientComponent.h"
#include "EntityManager.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "GuiComponent.h"
#include "LinkingContext.h"
#include "MapComponent.h"
#include "MapImporter.h"
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

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    ret = gameplayComponent.lock()->m_fsm.RegisterState<StartStateClient>();
    if (!ret) {
        return ret;
    }
    ret = gameplayComponent.lock()->m_fsm.RegisterState<PlayStateClient>();
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
    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    std::string path = MAPS_DIR;
    path.append(clientComponent.lock()->m_map);
    path.append(MAPS_EXTENSION);
    MapImporter::Tilemap tilemap;
    ret = g_gameClient->GetMapImporter().Load(path, tilemap);
    if (!ret) {
        return ret;
    }
    g_gameClient->GetMapImporter().Create(tilemap);

    std::weak_ptr<GuiComponent> guiComponent = g_gameClient->GetGuiComponent();
    guiComponent.lock()->m_isSettings = false;

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    gameplayComponent.lock()->m_mainMenuTimer.Start();
    ret = gameplayComponent.lock()->m_fsm.ChangeState("Start");

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::Update() const
{
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    return gameplayComponent.lock()->m_fsm.Update();
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::RenderGui() const
{
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    return gameplayComponent.lock()->m_fsm.RenderGui();
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::Exit() const
{
    // Scene
    g_gameClient->GetLinkingContext().ClearEntities();
    g_gameClient->GetEntityManager().ClearEntities();

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    clientComponent.lock()->Reset();

    clientComponent.lock()->m_entity = INVALID_ENTITY;

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> emptyState = std::weak_ptr<State>();
    gameplayComponent.lock()->m_fsm.ChangeState(emptyState);

    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    inputComponent.lock()->FullReset();

    std::weak_ptr<MapComponent> mapComponent = g_game->GetMapComponent();
    mapComponent.lock()->Reset();

    return true;
}

//----------------------------------------------------------------------------------------------------
void GameplayStateClient::OnNotify(const Event& event)
{
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    gameplayComponent.lock()->m_fsm.OnNotify(event);
}
}
