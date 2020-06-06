#include "GameplayStateServer.h"

#include "ClientProxy.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "GuiComponent.h"
#include "LinkingContext.h"
#include "MapComponent.h"
#include "MapImporter.h"
#include "PlayStateServer.h"
#include "ServerComponent.h"
#include "StartStateServer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string GameplayStateServer::GetName() const
{
    return "Gameplay";
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::Create() const
{
    bool ret = false;

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    ret = gameplayComponent.lock()->m_fsm.RegisterState<StartStateServer>();
    if (!ret) {
        return ret;
    }
    ret = gameplayComponent.lock()->m_fsm.RegisterState<PlayStateServer>();
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::Enter() const
{
    bool ret = false;

    // Scene
    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    std::string path = MAPS_DIR;
    path.append(serverComponent.lock()->m_map);
    path.append(MAPS_EXTENSION);
    MapImporter::Tilemap tilemap;
    ret = g_gameServer->GetMapImporter().Load(path, tilemap);
    if (!ret) {
        return ret;
    }
    g_gameServer->GetMapImporter().Create(tilemap);

    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.lock()->GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        std::shared_ptr<ClientProxy> clientProxy = pair.second;
        clientProxy->m_damageInflicted = 0;
        clientProxy->m_damageReceived = 0;
    }

    std::weak_ptr<GuiComponent> guiComponent = g_gameServer->GetGuiComponent();
    guiComponent.lock()->m_isSettings = false;

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    ret = gameplayComponent.lock()->m_fsm.ChangeState("Start");

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::Update() const
{
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    return gameplayComponent.lock()->m_fsm.Update();
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::RenderGui() const
{
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    return gameplayComponent.lock()->m_fsm.RenderGui();
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::Exit() const
{
    // Scene
    g_gameServer->GetLinkingContext().ClearEntities();
    g_gameServer->GetEntityManager().ClearEntities();

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.lock()->GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        PlayerID playerID = pair.first;
        std::shared_ptr<ClientProxy> clientProxy = pair.second;
        clientProxy->Reset();

        Entity entity = serverComponent.lock()->GetEntity(playerID);
        serverComponent.lock()->RemoveEntity(entity);
    }

    std::weak_ptr<MapComponent> mapComponent = g_game->GetMapComponent();
    mapComponent.lock()->Reset();

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> emptyState = std::weak_ptr<State>();
    gameplayComponent.lock()->m_fsm.ChangeState(emptyState);

    return true;
}

//----------------------------------------------------------------------------------------------------
void GameplayStateServer::OnNotify(const Event& event)
{
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    gameplayComponent.lock()->m_fsm.OnNotify(event);
}
}
