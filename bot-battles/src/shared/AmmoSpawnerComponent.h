#ifndef __AMMO_SPAWNER_COMPONENT_H__
#define __AMMO_SPAWNER_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct AmmoSpawnerComponent : public Component
#ifdef _CLIENT
    ,
                              public NetworkableReadObject
#elif defined(_SERVER)
    ,
                              public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::AMMO_SPAWNER; }

    AmmoSpawnerComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    // Local
    U32 m_ammo;
};
}

#endif
