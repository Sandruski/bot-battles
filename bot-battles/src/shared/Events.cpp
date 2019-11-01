#include "Events.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
Event::Event()
    : eventType(EventType::INVALID)
{
}

//----------------------------------------------------------------------------------------------------
Event::Event(const Event& event)
    : eventType(event.eventType)
{
    switch (eventType) {
    case EventType::ENTITY_REMOVED:
    case EventType::ENTITY_SIGNATURE_CHANGED: {
        entity.entity = event.entity.entity;
        entity.signature = event.entity.signature;
        break;
    }

    case EventType::COMPONENT_ADDED:
    case EventType::COMPONENT_REMOVED: {
        component.componentType = event.component.componentType;
        component.entity = event.component.entity;
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
Event::~Event()
{
}
}
