#ifndef __SCOREBOARD_COMPONENT_H__
#define __SCOREBOARD_COMPONENT_H__

#include "FSM.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct ScoreboardComponent {

    ScoreboardComponent();

    FSM m_fsm;
    U32 m_gameCount;
    PlayerID m_winnerPlayerID;
#ifdef _CLIENT
    MyTimer m_mainMenuTimer;
    MyTimer m_reHelloTimer;
    MyTimer m_guiTimer;
    F32 m_mainMenuTimeout;
    F32 m_secondsBetweenReHello;
#endif
};
}

#endif
