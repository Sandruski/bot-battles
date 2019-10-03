#include "ComponentTransform.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ComponentType ComponentTransform::GetType()
	{
		return ComponentType::TRANSFORM;
	}

	//----------------------------------------------------------------------------------------------------
	ComponentTransform::ComponentTransform(Entity* owner) : Component(owner),
		position()
	{
	}

	//----------------------------------------------------------------------------------------------------
	ComponentTransform::~ComponentTransform()
	{
	}
}