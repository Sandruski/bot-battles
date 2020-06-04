#include "PlayStateServer.h"

#include "ComponentManager.h"
#include "ConfigServer.h"
#include "EventComponent.h"
#include "FSM.h"
#include "GameServer.h"
#include "HealthComponent.h"
#include "ScoreboardComponent.h"
#include "ServerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string PlayStateServer::GetName() const
{
    return "Play";
}

//----------------------------------------------------------------------------------------------------
bool PlayStateServer::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    Event newEvent;
    newEvent.eventType = EventType::SEND_PLAY;
    g_gameServer->GetFSM().NotifyEvent(newEvent);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool PlayStateServer::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
void PlayStateServer::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::HEALTH_EMPTIED: {
        OnHealthEmptied(event.health.entity);
        break;
    }

    case EventType::PLAYER_REMOVED: {
        OnHealthEmptied(event.networking.entity);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void PlayStateServer::OnHealthEmptied(Entity entity) const
{
    U32 aliveCount = 0;
    Entity winnerEntity = INVALID_ENTITY;

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    std::vector<std::pair<Entity, std::weak_ptr<HealthComponent>>> healthComponents = g_gameServer->GetComponentManager().GetComponents<HealthComponent>();
    for (const auto& pair : healthComponents) {
        Entity ownerEntity = pair.first;
        PlayerID playerID = serverComponent.lock()->GetPlayerID(ownerEntity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        if (ownerEntity == entity) {
            continue;
        }

        std::weak_ptr<HealthComponent> healthComponent = pair.second;
        if (healthComponent.expired()) {
            continue;
        }

        const bool isAlive = healthComponent.lock()->m_currentHP > 0;
        if (isAlive) {
            ++aliveCount;
            winnerEntity = ownerEntity;
        }
    }

    // V X
    if (aliveCount == 0 || aliveCount == 1) {
        std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameServer->GetScoreboardComponent();
        scoreboardComponent.lock()->m_winnerPlayerID = serverComponent.lock()->GetPlayerID(winnerEntity);

        ChangeToScoreboard();
    }
}

//----------------------------------------------------------------------------------------------------
void PlayStateServer::ChangeToScoreboard() const
{
    g_gameServer->GetFSM().ChangeState("Scoreboard");
}
}
