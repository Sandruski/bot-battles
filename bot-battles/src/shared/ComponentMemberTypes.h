#ifndef __COMPONENT_MEMBER_TYPES_H__
#define __COMPONENT_MEMBER_TYPES_H__

namespace sand {

enum class InputComponentMemberType : U32 {

    INPUT_ACCELERATION = 1 << 0,
    INPUT_ANGULAR_ACCELERATION = 1 << 1,
    INPUT_SHOOTING = 1 << 2,

    COUNT,

    INPUT_ALL = INPUT_ACCELERATION | INPUT_ANGULAR_ACCELERATION | INPUT_SHOOTING
};

enum class ComponentMemberType : U32 {

    TRANSFORM_POSITION = 1 << 0,
    TRANSFORM_ROTATION = 1 << 1,
    TRANSFORM_VELOCITY = 1 << 2,
    TRANSFORM_ANGULAR_VELOCITY = 1 << 3,

    COLLIDER_SIZE = 1 << 4,

    HEALTH_HEALTH = 1 << 5,
    HEALTH_DEAD = 1 << 6,

#ifdef _DRAW
    SPRITE_ENABLED = 1 << 7,
    SPRITE_FILE = 1 << 8,
    SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS = 1 << 9,
    SPRITE_SPRITE_NAME = 1 << 10,
#endif

    COUNT,

    TRANSFORM_ALL = TRANSFORM_POSITION | TRANSFORM_ROTATION | TRANSFORM_VELOCITY | TRANSFORM_ANGULAR_VELOCITY,
    COLLIDER_ALL = COLLIDER_SIZE,
    HEALTH_ALL = HEALTH_HEALTH,
#ifdef _DRAW
    SPRITE_ALL = SPRITE_FILE | SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS | SPRITE_SPRITE_NAME,
    ALL = TRANSFORM_ALL | COLLIDER_ALL | HEALTH_ALL | SPRITE_ALL
#else
    ALL = TRANSFORM_ALL | COLLIDER_ALL | HEALTH_ALL
#endif
};
}

#endif
