#ifndef __SIGHT_COMPONENT_H__
#define __SIGHT_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct SightComponent : public Component
#ifdef _CLIENT
    ,
                        public NetworkableReadObject
#elif defined(_SERVER)
    ,
                        public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::SIGHT; }

    SightComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    // Networked
    F32 m_angle;
    F32 m_distance;

    // Local
#ifdef _SERVER
    Entity m_target;
#endif
};
}

#endif
