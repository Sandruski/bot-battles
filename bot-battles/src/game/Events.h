#ifndef __EVENTS_H__
#define __EVENTS_H__

#include "EntityDefs.h"
#include "ComponentDefs.h"

namespace sand
{

	enum class EventType {
		ENTITY_REMOVED,
		ENTITY_SIGNATURE_CHANGED,

		COMPONENT_ADDED,
		COMPONENT_REMOVED,

		COUNT,
		INVALID
	};

	struct EntityEvent
	{
		EventType type;
		Entity entity;
		unsigned long signature;
	};

	struct ComponentEvent
	{
		EventType type;
		Entity entity;
		ComponentType componentType;
	};

	//----------------------------------------------------------------------------------------------------
	union Event
	{
		EventType type;
		EntityEvent entity;
		ComponentEvent component;
	};
}

#endif
