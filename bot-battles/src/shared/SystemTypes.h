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
    CLIENT,
    INPUT,
    NAVIGATION,
    WEAPON,
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
    WEAPON,
    COLLISION,
#ifdef _DRAW
    HUD,
#endif
    COUNT
};
#endif
}

#endif
