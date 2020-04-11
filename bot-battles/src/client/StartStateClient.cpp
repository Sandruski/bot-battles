#include "StartStateClient.h"

#include "ConfigClient.h"
#include "FSM.h"
#include "GameClient.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string StartStateClient::GetName() const
{
    return "Start";
}

//----------------------------------------------------------------------------------------------------
bool StartStateClient::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool StartStateClient::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
void StartStateClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

        // V
    case EventType::PLAY_RECEIVED: {
        ChangeToPlay();
        break;
    }

        // X
    case EventType::PLAYER_REMOVED: {
        ChangeToMainMenu();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void StartStateClient::ChangeToPlay() const
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    gameplayComponent.m_fsm.ChangeState("Play");
}

//----------------------------------------------------------------------------------------------------
void StartStateClient::ChangeToMainMenu() const
{
    g_gameClient->GetFSM().ChangeState("Main Menu");
}
}
