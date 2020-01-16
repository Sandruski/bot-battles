#ifndef __SYSTEM_TYPES_H__
#define __SYSTEM_TYPES_H__

namespace sand {

enum class SystemType : U16 {

#ifdef _DRAW
    WINDOW,
    RENDERER,
    EVENT,
    HUD,
#endif

    NAVIGATION,

#ifdef _CLIENT
    CLIENT,
    INPUT,
#elif defined(_SERVER)
    SERVER,
    SPAWNER,
#endif

    COUNT
};
}

#endif
