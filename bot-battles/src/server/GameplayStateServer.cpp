#include "GameplayStateServer.h"

#include "ClientProxy.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "GuiComponent.h"
#include "LinkingContext.h"
#include "PlayStateServer.h"
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

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    ret = gameplayComponent.m_fsm.RegisterState<StartStateServer>();
    if (!ret) {
        return ret;
    }
    ret = gameplayComponent.m_fsm.RegisterState<PlayStateServer>();
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::Enter() const
{
    // Scene
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    std::string path = MAPS_DIR;
    path.append(serverComponent.m_map);
    path.append(MAPS_EXTENSION);
    MapImporter::Tilemap tilemap = g_gameServer->GetMapImporter().Load(path);
    g_gameServer->GetMapImporter().Create(tilemap);

    GuiComponent& guiComponent = g_gameServer->GetGuiComponent();
    guiComponent.m_isSettings = false;

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    return gameplayComponent.m_fsm.ChangeState("Start");
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::Exit() const
{
    // Scene
    g_gameServer->GetLinkingContext().ClearEntities();
    g_gameServer->GetEntityManager().ClearEntities();

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        PlayerID playerID = pair.first;
        std::shared_ptr<ClientProxy> clientProxy = pair.second;
        clientProxy->Reset();

        Entity entity = serverComponent.GetEntity(playerID);
        serverComponent.RemoveEntity(entity);
    }

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> emptyState = std::weak_ptr<State>();
    gameplayComponent.m_fsm.ChangeState(emptyState);

    return true;
}

//----------------------------------------------------------------------------------------------------
void GameplayStateServer::OnNotify(const Event& event)
{
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    gameplayComponent.m_fsm.OnNotify(event);
}
}
