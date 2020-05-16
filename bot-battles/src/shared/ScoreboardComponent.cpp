#include "ScoreboardComponent.h"

namespace sand {

// TODO: fill from config with scoreboard info (e.g. timers)

//----------------------------------------------------------------------------------------------------
ScoreboardComponent::ScoreboardComponent()
    : m_fsm()
    , m_gameCount(0)
    , m_winnerPlayerID(INVALID_PLAYER_ID)
#ifdef _CLIENT
    , m_mainMenuTimer()
    , m_reHelloTimer()
    , m_guiTimer()
    , m_mainMenuTimeout(30.0f)
    , m_secondsBetweenReHello(1.0f)
#endif
{
}
}
