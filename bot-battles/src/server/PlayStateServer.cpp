#include "PlayStateServer.h"

#include "ComponentManager.h"
#include "ConfigServer.h"
#include "GameServer.h"
#include "HealthComponent.h"

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
        OnHealthEmptied();
        break;
    }

        // X
    case EventType::PLAYER_REMOVED: {
        ChangeToScoreboard();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void PlayStateServer::OnHealthEmptied() const
{
    U32 aliveCount = 0;
    std::vector<std::pair<Entity, std::weak_ptr<HealthComponent>>> healthComponents = g_gameServer->GetComponentManager().GetComponents<HealthComponent>();
    for (const auto& pair : healthComponents) {
        std::weak_ptr<HealthComponent> healthComponent = pair.second;
        if (!healthComponent.lock()->IsDead()) {
            ++aliveCount;
        }
    }

    // V
    if (aliveCount == 1) {
        ChangeToScoreboard();
    }
}

//----------------------------------------------------------------------------------------------------
void PlayStateServer::ChangeToScoreboard() const
{
    g_gameServer->GetFSM().ChangeState(g_gameServer->GetConfig().m_scoreboardSceneName.c_str());
}
}
