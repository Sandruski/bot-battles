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

    REMOTE_PLAYER_MOVEMENT,

    INPUT,

    MOVEMENT,
    COLLISION,
    WEAPON,

#ifdef _DRAW
    GUI,
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
    MOVEMENT,
    COLLISION,
    WEAPON,
    HEALTH,

    INPUT,
#ifdef _DRAW
    GUI,
#endif
    COUNT
};
#endif
}

#endif
