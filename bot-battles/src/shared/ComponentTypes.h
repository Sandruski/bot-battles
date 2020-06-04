#ifndef __COMPONENT_TYPES_H__
#define __COMPONENT_TYPES_H__

namespace sand {

enum class ComponentType : U16 {

    // ENGINE
    TRANSFORM,
    COLLIDER,
    RIGIDBODY,
    LABEL,
    SPRITE,
    PLAYER,

    // GAMEPLAY
    WEAPON,
    HEALTH,
    SIGHT,
    BOT,
    WALL,
    BOT_SPAWNER,
    WEAPON_SPAWNER,
    HEALTH_SPAWNER,

#ifdef _CLIENT
    LOCAL_PLAYER,
    REMOTE_PLAYER,
#endif

    COUNT
};
}

#endif
