#ifndef __EVENT_TYPES_H__
#define __EVENT_TYPES_H__

namespace sand {

#ifdef _CLIENT
enum class EventType {
    ENTITY_REMOVED,
    ENTITY_SIGNATURE_CHANGED,

    COMPONENT_ADDED,
    COMPONENT_REMOVED,

    COUNT,
    INVALID
};
#elif _SERVER
enum class EventType {
    ENTITY_REMOVED,
    ENTITY_SIGNATURE_CHANGED,

    COMPONENT_ADDED,
    COMPONENT_REMOVED,

    COUNT,
    INVALID
};
#endif
}

#endif
