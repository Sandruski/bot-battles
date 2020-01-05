#ifndef __EVENTS_H__
#define __EVENTS_H__

#include "ComponentDefs.h"
#include "EntityDefs.h"
#include "EventTypes.h"
#include "NetDefs.h"

namespace sand {

struct EntityEvent {
    EventType eventType;
    Entity entity;
    Signature signature;
};

struct ComponentEvent {
    EventType eventType;
    ComponentType componentType;
    U32 dirtyState;
    Entity entity;
};

struct NetworkingEvent {
    EventType eventType;
    PlayerID playerID;
    Entity entity;
};

//----------------------------------------------------------------------------------------------------
union Event {
    EventType eventType;
    EntityEvent entity;
    ComponentEvent component;
    NetworkingEvent networking;
};
}

#endif
