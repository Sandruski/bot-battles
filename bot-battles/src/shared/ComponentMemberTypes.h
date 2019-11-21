#ifndef __COMPONENT_MEMBER_TYPES_H__
#define __COMPONENT_MEMBER_TYPES_H__

namespace sand {

enum class ComponentMemberType : U32 {
    INPUT_ACCELERATION = 1 << 0,
    INPUT_ANGULAR_ACCELERATION = 1 << 1,

    TRANSFORM_POSITION = 1 << 2,
    TRANSFORM_ROTATION = 1 << 3,

    COUNT,
    INVALID,

    INPUT_ALL = INPUT_ACCELERATION | INPUT_ANGULAR_ACCELERATION,
    TRANSFORM_ALL = TRANSFORM_POSITION | TRANSFORM_ROTATION,
    ALL = INPUT_ALL | TRANSFORM_ALL
};
}

#endif
