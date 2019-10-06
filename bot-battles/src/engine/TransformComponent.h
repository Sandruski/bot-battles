#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "Component.h"

#include "Vector3.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	struct TransformComponent : public Component
	{
		static ComponentType GetType() { return ComponentType::TRANSFORM; }

		TransformComponent(ComponentID id) : Component(id) {}
		~TransformComponent() override {}

		Vector3 m_position;
	};
}

#endif