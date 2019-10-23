#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "Vec2.h"
#include "MemoryStream.h"

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

	// TODO: change this method
	void Write(OutputMemoryStream& outMemoryStream) const
	{
		// Serialize
		outMemoryStream.WritePosition(m_position);
	}

	Vec2 m_position;
};
}

#endif
