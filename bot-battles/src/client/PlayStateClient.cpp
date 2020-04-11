#include "PlayStateClient.h"

#include "ConfigClient.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameClient.h"
#include "LinkingContext.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string PlayStateClient::GetName() const
{
    return "Play";
}

//----------------------------------------------------------------------------------------------------
bool PlayStateClient::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool PlayStateClient::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
void PlayStateClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

        // V
    case EventType::RESULTS_RECEIVED: {
        ChangeToScoreboard();
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
void PlayStateClient::ChangeToScoreboard() const
{
    g_gameClient->GetFSM().ChangeState("Scoreboard");
}

//----------------------------------------------------------------------------------------------------
void PlayStateClient::ChangeToMainMenu() const
{
    g_gameClient->GetFSM().ChangeState("Main Menu");
}
}
