#ifndef __SYSTEM_TYPES_H__
#define __SYSTEM_TYPES_H__

namespace sand {

#ifdef _CLIENT
enum class SystemType : U16 {
#ifdef _DRAW
    WINDOW,
    RENDERER,
    EVENT,
#endif
    INPUT,

    NAVIGATION,
    COLLISION,
    WEAPON,

    CLIENT,
#ifdef _DRAW
    HUD,
#endif
    COUNT
};
#elif defined(_SERVER)
enum class SystemType : U16 {
#ifdef _DRAW
    WINDOW,
    RENDERER,
    EVENT,
#endif
    SERVER,

    SPAWNER,
    NAVIGATION,
    COLLISION,
    WEAPON,

    INPUT,
#ifdef _DRAW
    HUD,
#endif
    COUNT
};
#endif
}

#endif
