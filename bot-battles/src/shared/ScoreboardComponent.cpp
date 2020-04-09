#include "ScoreboardComponent.h"

namespace sand {

// TODO: fill from config with gameplay info

//----------------------------------------------------------------------------------------------------
ScoreboardComponent::ScoreboardComponent()
    : m_fsm()
    , m_gameCount(0)
    , m_winnerPlayerID(INVALID_PLAYER_ID)
#ifdef _CLIENT
    , m_mainMenuTimer()
    , m_reHelloTimer()
    , m_guiTimer()
    , m_mainMenuTimeout(15.0f)
#endif
{
}
}
