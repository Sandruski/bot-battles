#ifndef __HEALTH_COMPONENT_H__
#define __HEALTH_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct HealthComponent : public Component
#ifdef _CLIENT
    ,
                         public NetworkableReadObject
#elif defined(_SERVER)
    ,
                         public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::HEALTH; }

    HealthComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    I32 m_health;
    bool m_isDead;
};
}

#endif
