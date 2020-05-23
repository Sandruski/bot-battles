#ifndef __EVENT_TYPES_H__
#define __EVENT_TYPES_H__

namespace sand {

enum class EventType {

    // ENGINE
    // Non-networking
    ENTITY_ADDED,
    ENTITY_REMOVED,
    ENTITY_SIGNATURE_CHANGED,

    COMPONENT_ADDED,
    COMPONENT_REMOVED,
    COMPONENT_MEMBER_CHANGED,

    COLLISION_ENTER,
    COLLISION_EXIT,

    // Networking
    NETWORK_ENTITY_ADDED,
    NETWORK_ENTITY_REMOVED,

    PLAYER_ADDED,
    PLAYER_READDED,
    PLAYER_REMOVED,

    PLAYER_ENTITY_ADDED,

    CONNECT_SOCKETS,
    SOCKETS_CONNECTED,

    TRY_CONNECT,
    CONNECT_SUCCESSFUL,
    CONNECT_FAILED,

#ifdef _CLIENT
    CHECK_CONNECT,

    SEND_HELLO,
    SEND_REHELLO,

    WELCOME_RECEIVED,
    UNWELCOME_RECEIVED,
    REWELCOME_RECEIVED,
    PLAY_RECEIVED,
    RESULTS_RECEIVED,

    DISCONNECT_SOCKETS,
#elif defined(_SERVER)
    HELLO_RECEIVED,
    REHELLO_RECEIVED,

    SEND_WELCOME,
    SEND_REWELCOME,
    SEND_PLAY,
    SEND_RESULTS,
#endif

    SEND_BYE,

    // GAMEPLAY
    // Weapon
    WEAPON_HIT,

    // Health
    HEALTH_EMPTIED,

    // Sight
    SEEN_NEW_ENTITY,
    SEEN_LOST_ENTITY,

    COUNT
};
}

#endif
