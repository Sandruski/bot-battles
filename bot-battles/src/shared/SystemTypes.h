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

#ifdef _CLIENT
    CLIENT,
    INPUT,
#elif _SERVER
    SERVER,
    NAVIGATION,
    SPAWNER,
#endif

    COUNT
};
}

#endif
