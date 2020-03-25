#ifndef __GAMEPLAY_COMPONENT_H__
#define __GAMEPLAY_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct GameplayComponent {

    enum class GameplayPhase : U16 {

        START,
        PLAY,

        NONE
    };

    GameplayComponent();

    MyTimer m_timer;
    GameplayPhase m_phase;
};
}

#endif
