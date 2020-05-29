#ifndef __BOT_SPAWNER_COMPONENT_H__
#define __BOT_SPAWNER_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct BotSpawnerComponent : public Component
#ifdef _CLIENT
    ,
                             public NetworkableReadObject
#elif defined(_SERVER)
    ,
                             public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::BOT_SPAWNER; }

    BotSpawnerComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U64 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U64 Write(OutputMemoryStream& outputStream, U64 dirtyState) const override;
#endif

    // Local
    F32 m_facing;
    F32 m_sightAngle;
    F32 m_sightDistance;
    PlayerID m_playerID;
    U32 m_health;

    U32 m_damageWeapon;
    F32 m_rangeWeapon;
    F32 m_cooldownWeapon;
};
}

#endif
