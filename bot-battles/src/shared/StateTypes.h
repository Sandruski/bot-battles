#ifndef __STATE_TYPES_H__
#define __STATE_TYPES_H__

namespace sand {

enum class StateType : U16 {
#ifdef _CLIENT
    MAIN_MENU,
#endif
    GAMEPLAY,

    COUNT
};
}

#endif
