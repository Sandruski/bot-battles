#ifndef __COMPONENT_TRANSFORM_H__
#define __COMPONENT_TRANSFORM_H__

#include "Component.h"
#include "ComponentTypes.h"

#include "Vector3.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class ComponentTransform : public Component
	{
	public:
		static ComponentType GetType();

	public:
		ComponentTransform(Entity* owner);
		~ComponentTransform() override;

	public:
		Vector3 position;
	};
}

#endif
