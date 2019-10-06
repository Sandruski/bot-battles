#ifndef __COMPONENT_TRANSFORM_H__
#define __COMPONENT_TRANSFORM_H__

#include "Component.h"

#include "Vector3.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	struct ComponentTransform : public Component
	{
		static ComponentType GetType() { return ComponentType::TRANSFORM; }

		ComponentTransform(ComponentID id) : Component(id) {}
		~ComponentTransform() override {}

		Vector3 m_position;
	};
}

#endif