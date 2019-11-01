#ifndef __COMPONENT_TYPES_H__
#define __COMPONENT_TYPES_H__

namespace sand {

#ifdef _CLIENT
enum class ComponentType {
    TRANSFORM,
    SPRITE,

    COUNT,
    INVALID
};
#elif _SERVER
enum class ComponentType {
    TRANSFORM,

    COUNT,
    INVALID
};
#endif
}

#endif
