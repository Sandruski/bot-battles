#ifndef __SYSTEM_TYPES_H__
#define __SYSTEM_TYPES_H__

namespace sand {

enum class SystemType : U16 {

#ifdef _CLIENT
#ifdef _DRAW
    EVENT,
    WINDOW,
#endif
    CLIENT,

    REMOTE_PLAYER_MOVEMENT,

    SCRIPTING,
    INPUT,

    PHYSICS,
    //
    SIGHT,
    MOVEMENT,
    WEAPON,
    //

    COLLISION, // TODO

    OUTPUT,

#ifdef _DRAW
    RENDERER,
    HUD,
    GUI,
#endif

#elif defined(_SERVER)
#ifdef _DRAW
    EVENT,
    WINDOW,
#endif
    SERVER,

    SPAWNER,
    PHYSICS,
    //
    SIGHT,
    MOVEMENT,
    AMMO,
    WEAPON,
    HEALTH,
    //

    COLLISION, // TODO

    OUTPUT,

#ifdef _DRAW
    RENDERER,
    HUD,
    GUI,
#endif
#endif

    COUNT
};
}

#endif
