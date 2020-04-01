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
#ifdef _CLIENT
    MyTimer m_mainMenuTimer;
    MyTimer m_guiTimer;
    F32 m_mainMenuTimeout;
#endif
};
}

#endif
