#ifndef __COMPONENT_TYPES_H__
#define __COMPONENT_TYPES_H__

namespace sand {

enum class ComponentType : U16 {

    TRANSFORM,
    INPUT,

#ifdef _DRAW
    SPRITE,
    TEXT,
#endif

    COUNT
};
}

#endif
