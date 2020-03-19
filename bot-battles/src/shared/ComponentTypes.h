#ifndef __COMPONENT_TYPES_H__
#define __COMPONENT_TYPES_H__

namespace sand {

enum class ComponentType : U16 {

    TRANSFORM,
    WEAPON,
    COLLIDER,
    HEALTH,
    MAP,

#ifdef _DRAW
    SPRITE,
    MESH,
#endif

#ifdef _CLIENT
    LOCAL_PLAYER,
    REMOTE_PLAYER,
#endif

    COUNT
};
}

#endif
