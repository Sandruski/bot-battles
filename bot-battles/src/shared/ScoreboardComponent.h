#ifndef __SCOREBOARD_COMPONENT_H__
#define __SCOREBOARD_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct ScoreboardComponent {

    enum class ScoreboardPhase : U16 {

        RESULTS,
        RESTART,

        NONE
    };

    ScoreboardComponent();

    F32 m_mainMenuTimeout;

    MyTimer m_timer;
    ScoreboardPhase m_phase;
};
}

#endif
