#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "Vec2.h"

namespace sand {

	class OutputMemoryStream;

//----------------------------------------------------------------------------------------------------
struct TransformComponent {
    static ComponentType GetType() { return ComponentType::TRANSFORM; }

    TransformComponent()
        : m_position()
    {
    }
    ~TransformComponent() { }

	//void Write(OutputMemoryStream& inStream) const;

	Vec2 m_position;
};
}

#endif
