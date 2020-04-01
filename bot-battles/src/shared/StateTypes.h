#ifndef __STATE_TYPES_H__
#define __STATE_TYPES_H__

namespace sand {

enum class StateType : U16 {

    MAIN_MENU,
    SETUP,
    CONNECT,

    GAMEPLAY,
    START,
    PLAY,

    SCOREBOARD,
    RESULTS,
    RESTART,

    COUNT
};
}

#endif
