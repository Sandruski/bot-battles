#ifndef __GAME_COMPONENT_H__
#define __GAME_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct GameComponent {

    GameComponent();

    PhaseType m_phaseType;
    MyTimer m_timer;
};
}

#endif
