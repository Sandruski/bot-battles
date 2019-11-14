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
    Entity entity;
};

struct ServerEvent {
    EventType eventType;
    PlayerID playerID;
};

//----------------------------------------------------------------------------------------------------
union Event {
    EventType eventType;
    EntityEvent entity;
    ComponentEvent component;
    ServerEvent server;
};
}

#endif
