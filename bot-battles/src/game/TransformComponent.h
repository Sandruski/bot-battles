#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "Vec3.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct TransformComponent {
    static ComponentType GetType() { return ComponentType::TRANSFORM; }

    TransformComponent()
        : m_position()
    {
    }
    ~TransformComponent() { }

    Vec3 m_position;
};
}

#endif
