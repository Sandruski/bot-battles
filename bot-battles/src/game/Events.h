#ifndef __EVENTS_H__
#define __EVENTS_H__

#include "EntityDefs.h"

namespace sand
{

	enum class EventType {
		ENTITY_REMOVED,
		COMPONENT_ADDED,
		COMPONENT_REMOVED,

		COUNT,
		INVALID
	};

	struct EntityEvent
	{
		EventType type;
		Entity entity;
	};

	//----------------------------------------------------------------------------------------------------
	union Event
	{
		EventType type;
		EntityEvent entity;
	};
}

#endif
