#ifndef __EVENT_TYPES_H__
#define __EVENT_TYPES_H__

namespace sand {

enum class EventType {
    ENTITY_ADDED,
    ENTITY_REMOVED,
    ENTITY_SIGNATURE_CHANGED,

    COMPONENT_ADDED,
    COMPONENT_REMOVED,
    COMPONENT_MEMBER_CHANGED,

#ifdef _CLIENT
#elif _SERVER
    PLAYER_ADDED,
#endif

    COUNT,
    INVALID
};
}

#endif
