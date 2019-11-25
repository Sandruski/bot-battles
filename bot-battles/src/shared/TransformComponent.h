#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "NetComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct TransformComponent : public NetComponent {

    static ComponentType GetType() { return ComponentType::TRANSFORM; }
    static TransformComponent* Instantiate() { return new TransformComponent(); }

	TransformComponent();
	~TransformComponent();

	U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
	void Read(InputMemoryStream& inputStream, U32 dirtyState) override;

    Vec2 m_position;
    float m_rotation;
};
}

#endif
