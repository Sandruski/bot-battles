#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "EntityDefs.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	struct Entity
	{
		Entity(EntityID id) : m_id(id) {}
		~Entity() {}

		EntityID m_id;
	};
}

#endif