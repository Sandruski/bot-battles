#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "ComponentDefs.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	struct Component
	{
		static ComponentType GetType() { return ComponentType::COMPONENT_INVALID; }

		Component(ComponentID id) : m_id(id) {}
		virtual ~Component() {}

		ComponentID m_id;
	};
}

#endif