#ifndef __GAMEPLAY_COMPONENT_H__
#define __GAMEPLAY_COMPONENT_H__

#include "FSM.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct GameplayComponent {

    GameplayComponent();

    FSM m_fsm;
};
}

#endif
