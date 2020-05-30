#ifndef __COMPONENT_TYPES_H__
#define __COMPONENT_TYPES_H__

namespace sand {

enum class ComponentType : U16 {

    TRANSFORM,
    WEAPON,
    COLLIDER,
    RIGIDBODY,
    HEALTH,
    BOT,
    WALL,
    SIGHT,
    BOT_SPAWNER,
    WEAPON_SPAWNER,
    HEALTH_SPAWNER,
    PLAYER,

#ifdef _DRAW
    LABEL,
    SPRITE,
#endif

#ifdef _CLIENT
    LOCAL_PLAYER,
    REMOTE_PLAYER,
#endif

    COUNT
};
}

#endif
