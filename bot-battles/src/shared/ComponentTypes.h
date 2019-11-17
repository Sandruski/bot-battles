#ifndef __COMPONENT_TYPES_H__
#define __COMPONENT_TYPES_H__

namespace sand {

#ifdef _CLIENT
enum class ComponentType : U16 {
    TRANSFORM,
    INPUT,

    SPRITE,

    COUNT,
    INVALID
};
#elif _SERVER
enum class ComponentType : U16 {
    TRANSFORM,
    INPUT,

    COUNT,
    INVALID
};
#endif
}

#endif
