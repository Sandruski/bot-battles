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

    SCRIPTING,
    INPUT,

    PHYSICS,
    COLLISION,

    //
    SIGHT,
    MOVEMENT,
    WEAPON,
    HEALTH,
    //

    MAP,

    OUTPUT,

#ifdef _DRAW
    LABEL,
    GUI,
#endif

#elif defined(_SERVER)
#ifdef _DRAW
    EVENT,
    WINDOW,
    RENDERER,
#endif
    SERVER,

    BOT_SPAWNER,
    WEAPON_SPAWNER,
    HEALTH_SPAWNER,

    PHYSICS,
    COLLISION,

    //
    SIGHT,
    MOVEMENT,
    WEAPON,
    HEALTH,
    //

    BOT,
    MAP,

    OUTPUT,

#ifdef _DRAW
    LABEL,
    GUI,
#endif
#endif

    COUNT
};
}

#endif
