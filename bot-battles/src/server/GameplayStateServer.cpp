#include "GameplayStateServer.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "Config.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "HealthComponent.h"
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
    ILOG("Entering %s...", GetName());

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    gameplayComponent.m_phase = GameplayComponent::GameplayPhase::START;

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    std::string path;
    path.append(MAPS_DIR);
    path.append(serverComponent.m_map);
    MapImporter::Tilemap tilemap = g_gameServer->GetMapImporter().Load(path);
    g_gameServer->GetMapImporter().Create(tilemap);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateServer::Exit() const
{
    ILOG("Exiting %s...", GetName());

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    gameplayComponent.m_phase = GameplayComponent::GameplayPhase::NONE;

    g_game->GetLinkingContext().ClearEntities();
    g_game->GetEntityManager().ClearEntities();

    // TODO: do it through the event PLAYER_REMOVED
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        PlayerID playerID = pair.first;
        Entity entity = serverComponent.GetEntity(playerID);
        serverComponent.RemoveEntity(entity);
        std::shared_ptr<ClientProxy> clientProxy = pair.second;
        clientProxy->Reset();
    }

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

    case EventType::HEALTH_EMPTIED: {
        OnHealthEmptied();
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
    U32 entityCount = serverComponent.GetEntityCount();
    if (entityCount == MAX_PLAYER_IDS) {
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
        // TODO: restart the players' positions, etc.
        // TODO: restart the global parameters of the match.
        //g_gameServer->GetFSM().ChangeState(g_gameServer->GetConfig().m_onlineSceneName.c_str()); // if new objects are spawned throughout the gameplay
    }
}

//----------------------------------------------------------------------------------------------------
void GameplayStateServer::OnHealthEmptied() const
{
    U32 aliveCount = 0;
    std::vector<std::pair<Entity, std::weak_ptr<HealthComponent>>> healthComponents = g_gameServer->GetComponentManager().GetComponents<HealthComponent>();
    for (const auto& pair : healthComponents) {
        std::weak_ptr<HealthComponent> healthComponent = pair.second;
        if (!healthComponent.lock()->IsDead()) {
            ++aliveCount;
        }
    }

    if (aliveCount == 1) {
        GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
        if (gameplayComponent.m_phase == GameplayComponent::GameplayPhase::PLAY) {
            g_gameServer->GetFSM().ChangeState("Scoreboard"); // TODO: config
        }
    }
}
}
