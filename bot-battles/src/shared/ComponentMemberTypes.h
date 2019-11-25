#ifndef __COMPONENT_MEMBER_TYPES_H__
#define __COMPONENT_MEMBER_TYPES_H__

namespace sand {

enum class ComponentMemberType : U32 {
    INPUT_ACCELERATION = 1 << 0,
    INPUT_ANGULAR_ACCELERATION = 1 << 1,

    TRANSFORM_POSITION = 1 << 2,
    TRANSFORM_ROTATION = 1 << 3,

#ifdef _DRAW
    SPRITE_FILE = 1 << 4,
	TEXT_STUFF = 1 << 5,
#endif

    COUNT,
    INVALID,

    INPUT_ALL = INPUT_ACCELERATION | INPUT_ANGULAR_ACCELERATION,
    TRANSFORM_ALL = TRANSFORM_POSITION | TRANSFORM_ROTATION,
#ifdef _DRAW
    SPRITE_ALL = SPRITE_FILE,
    ALL = INPUT_ALL | TRANSFORM_ALL | SPRITE_ALL
#else
    ALL = INPUT_ALL | TRANSFORM_ALL
#endif
};
}

#endif
