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

    COLLIDER_POSITION = 1 << 4,
    COLLIDER_SIZE = 1 << 5,

#ifdef _DRAW
    SPRITE_FILE = 1 << 6,
    SPRITE_SPRITES = 1 << 7,
    SPRITE_CURRENT_SPRITE = 1 << 8,
    TEXT_STUFF = 1 << 9,
#endif

    COUNT,

    TRANSFORM_ALL = TRANSFORM_POSITION | TRANSFORM_ROTATION | TRANSFORM_VELOCITY | TRANSFORM_ANGULAR_VELOCITY,
    COLLIDER_ALL = COLLIDER_POSITION | COLLIDER_SIZE,
#ifdef _DRAW
    SPRITE_ALL = SPRITE_FILE | SPRITE_SPRITES | SPRITE_CURRENT_SPRITE,
    TEXT_ALL = TEXT_STUFF,
    ALL = TRANSFORM_ALL | COLLIDER_ALL | SPRITE_ALL | TEXT_ALL
#else
    ALL = TRANSFORM_ALL | COLLIDER_ALL
#endif
};
}

#endif
