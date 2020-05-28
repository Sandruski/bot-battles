#ifndef __COMPONENT_TYPES_H__
#define __COMPONENT_TYPES_H__

namespace sand {

enum class ComponentType : U16 {

    TRANSFORM,
    WEAPON,
    COLLIDER,
    RIGIDBODY,
    HEALTH,
    BOT_SPAWNER,
    SPAWNER, // TODO: you can only be a spawner at once
    BOT,
    WALL,
    SIGHT,

#ifdef _DRAW
    LABEL,
    SPRITE,
#endif

#ifdef _CLIENT
    LOCAL_PLAYER, // client-only
    REMOTE_PLAYER, // client-only
#elif defined(_SERVER)
    PLAYER, // server-only
#endif

    COUNT
};
}

#endif
