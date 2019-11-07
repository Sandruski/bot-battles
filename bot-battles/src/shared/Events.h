#ifndef __EVENTS_H__
#define __EVENTS_H__

#include "ComponentDefs.h"
#include "EntityDefs.h"
#include "EventTypes.h"

namespace sand {

struct EntityEvent {
    EventType eventType;
    Entity entity;
    Signature signature;
};

struct ComponentEvent {
    EventType eventType;
    ComponentType componentType;
    Entity entity;
};

//----------------------------------------------------------------------------------------------------
union Event {
    EventType eventType;
    EntityEvent entity;
    ComponentEvent component;
};
}

#endif
