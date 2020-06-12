#ifndef __SYSTEM_TYPES_H__
#define __SYSTEM_TYPES_H__

namespace sand {

enum class SystemType : U16 {

#ifdef _CLIENT
    EVENT,
    WINDOW,
    RENDERER,

    CLIENT,

    REMOTE_PLAYER_MOVEMENT,
    REMOTE_PLAYER_WEAPON,

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

    LABEL,
    GUI,

#elif defined(_SERVER)
    EVENT,
    WINDOW,
    RENDERER,

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

    LABEL,
    GUI,
#endif

    COUNT
};
}

#endif
