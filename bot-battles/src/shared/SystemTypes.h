#ifndef __SYSTEM_TYPES_H__
#define __SYSTEM_TYPES_H__

namespace sand {

enum class SystemType : U16 {

#ifdef _CLIENT
#ifdef _DRAW
    EVENT,
    WINDOW,
    RENDERER,
#endif
    CLIENT,

    REMOTE_PLAYER_MOVEMENT,

    SIGHT,

    SCRIPTING,
    INPUT,

    PHYSICS,
    //
    MOVEMENT,
    WEAPON,
    //

    COLLISION, // TODO

    OUTPUT,

#ifdef _DRAW
    HUD,
    GUI,
#endif

#elif defined(_SERVER)
#ifdef _DRAW
    EVENT,
    WINDOW,
    RENDERER,
#endif
    SERVER,

    SPAWNER,
    PHYSICS,
    //
    MOVEMENT,
    WEAPON,
    HEALTH,
    //

    COLLISION, // TODO

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
