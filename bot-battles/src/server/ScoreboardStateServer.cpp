#include "ScoreboardStateServer.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "ConfigServer.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameServer.h"
#include "ScoreboardComponent.h"
#include "ServerComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
const char* ScoreboardStateServer::GetName() const
{
    return "Scoreboard";
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateServer::Enter() const
{
    ILOG("Entering %s...", GetName());

    ScoreboardComponent& scoreboardComponent = g_gameServer->GetScoreboardComponent();
    scoreboardComponent.m_phase = ScoreboardComponent::ScoreboardPhase::RESULTS;
    ++scoreboardComponent.m_gameCount;

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        std::shared_ptr<ClientProxy> clientProxy = pair.second;
        clientProxy->m_sendResultPacket = true;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateServer::Update() const
{
    ScoreboardComponent& scoreboardComponent = g_gameServer->GetScoreboardComponent();
    switch (scoreboardComponent.m_phase) {
    case ScoreboardComponent::ScoreboardPhase::RESULTS: {
        UpdateResults(scoreboardComponent);
        break;
    }

    case ScoreboardComponent::ScoreboardPhase::RESTART: {
        UpdateRestart();
        break;
    }

    default: {
        break;
    }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateServer::Exit() const
{
    ILOG("Exiting %s...", GetName());

    ScoreboardComponent& scoreboardComponent = g_gameServer->GetScoreboardComponent();
    scoreboardComponent.m_phase = ScoreboardComponent::ScoreboardPhase::NONE;

    return true;
}

//----------------------------------------------------------------------------------------------------
void ScoreboardStateServer::UpdateResults(ScoreboardComponent& scoreboardComponent) const
{
    scoreboardComponent.m_phase = ScoreboardComponent::ScoreboardPhase::RESTART;
}

//----------------------------------------------------------------------------------------------------
void ScoreboardStateServer::UpdateRestart() const
{
    g_gameServer->GetFSM().ChangeState(g_gameServer->GetConfig().m_onlineSceneName.c_str());
}
}
