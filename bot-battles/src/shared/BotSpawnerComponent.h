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
    void Read(InputMemoryStream& inputStream, U32 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    // Local
    PlayerID m_playerID;
    F32 m_facing;
};
}

#endif
