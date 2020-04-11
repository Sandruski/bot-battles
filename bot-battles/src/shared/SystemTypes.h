#ifndef __SYSTEM_TYPES_H__
#define __SYSTEM_TYPES_H__

namespace sand {

enum class SystemType : U16 {

#ifdef _CLIENT
#ifdef _DRAW
    WINDOW,
    RENDERER,
    EVENT,
#endif
    CLIENT,

    REMOTE_PLAYER_MOVEMENT,

    INPUT,

    MOVEMENT,
    COLLISION,
    WEAPON,

    OUTPUT,

#ifdef _DRAW
    HUD,
    GUI,
#endif

#elif defined(_SERVER)
#ifdef _DRAW
    WINDOW,
    RENDERER,
    EVENT,
#endif
    SERVER,

    SPAWNER,
    MOVEMENT,
    COLLISION,
    WEAPON,
    HEALTH,

    OUTPUT,

#ifdef _DRAW
    HUD,
    GUI,
#endif
#endif

    COUNT
};
}

#endif
