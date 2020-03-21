#ifndef __SYSTEM_TYPES_H__
#define __SYSTEM_TYPES_H__

namespace sand {

#ifdef _CLIENT
enum class SystemType : U16 {
#ifdef _DRAW
    WINDOW,
    RENDERER,
    MAP_RENDERER,
    GUI,
    EVENT,
#endif
    CLIENT,

    REMOTE_PLAYER_MOVEMENT,

    INPUT,

    MOVEMENT,
    COLLISION,
    WEAPON,

    COUNT
};
#elif defined(_SERVER)
enum class SystemType : U16 {
#ifdef _DRAW
    WINDOW,
    RENDERER,
    MAP_RENDERER,
    GUI,
    EVENT,
#endif
    SERVER,

    SPAWNER,
    MOVEMENT,
    COLLISION,
    WEAPON,
    HEALTH,

    INPUT,

    COUNT
};
#endif
}

#endif
