#ifndef __EVENTS_H__
#define __EVENTS_H__

#include "ComponentDefs.h"
#include "EntityDefs.h"
#include "EventTypes.h"
#include "NetDefs.h"
#include "SystemDefs.h"

namespace sand {

// ENGINE
// Non-networking
struct EntityEvent {
    EventType eventType;
    Entity entity;
    Signature signature;
};

struct ComponentEvent {
    EventType eventType;
    ComponentType componentType;
    U64 dirtyState;
    Entity entity;
};

struct SystemEvent {
    EventType eventType;
    SystemType systemType;
    Entity entity;
};

struct CollisionEvent {
    EventType eventType;
    Entity entityA;
    Entity entityB;
    glm::vec2 linearVelocityA;
    glm::vec2 linearVelocityB;
    glm::vec2 normal;
};

// Networking
struct NetworkingEvent {
    EventType eventType;
    PlayerID playerID;
    Entity entity;
    NetworkID networkID;
};

// GAMEPLAY
// Weapon
struct WeaponEvent {
    EventType eventType;
    Entity shooterEntity;
    Entity targetEntity;
    U32 damage;
    U32 ammo;
};

// Health
struct HealthEvent {
    EventType eventType;
    Entity entity;
    U32 health;
};

// Sight
struct SightEvent {
    EventType eventType;
    PlayerID playerID;
    Entity entity;
};

//----------------------------------------------------------------------------------------------------
union Event {
    EventType eventType;
    EntityEvent entity;
    ComponentEvent component;
    SystemEvent system;
    NetworkingEvent networking;
    CollisionEvent collision;
    WeaponEvent weapon;
    HealthEvent health;
    SightEvent sight;
};
}

#endif
