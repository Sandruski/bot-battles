#include "StartStateServer.h"

#include "GameServer.h"
#include "ServerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string StartStateServer::GetName() const
{
    return "Start";
}

//----------------------------------------------------------------------------------------------------
bool StartStateServer::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool StartStateServer::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
void StartStateServer::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::PLAYER_ADDED: {
        OnPlayerAdded();
        break;
    }

    case EventType::HELLO_RECEIVED: {
        Event newEvent;
        newEvent.eventType = EventType::SEND_WELCOME;
        newEvent.networking.playerID = event.networking.playerID;
        g_gameServer->GetFSM().NotifyEvent(newEvent);
        break;
    }

    case EventType::REHELLO_RECEIVED: {
        Event newEvent;
        newEvent.eventType = EventType::SEND_REWELCOME;
        newEvent.networking.playerID = event.networking.playerID;
        g_gameServer->GetFSM().NotifyEvent(newEvent);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void StartStateServer::OnPlayerAdded() const
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    U32 entityCount = serverComponent.GetEntityCount();
    // V
    if (entityCount == MAX_PLAYER_IDS) {
        ChangeToPlay();
    }
}

//----------------------------------------------------------------------------------------------------
void StartStateServer::ChangeToPlay() const
{
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    gameplayComponent.m_fsm.ChangeState("Play");
}
}
