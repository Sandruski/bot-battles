#ifndef __COLLIDER_COMPONENT_H__
#define __COLLIDER_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct ColliderComponent : public Component
#ifdef _CLIENT
    ,
                           public NetworkableReadObject
#elif defined(_SERVER)
    ,
                           public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::COLLIDER; }

    enum class ShapeType : U8 {

        NONE,
        CIRCLE,
        BOX
    };

    ColliderComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U64 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U64 Write(OutputMemoryStream& outputStream, U64 dirtyState) const override;
#endif

    // Networked
    glm::vec2 m_size;
    ShapeType m_shapeType;
    bool m_isTrigger;
};
}

#endif
