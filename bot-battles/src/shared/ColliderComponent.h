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

    enum class ShapeType {
        BOX,
        CIRCLE
    };

    ColliderComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    glm::vec2 GetMin() const;
    glm::vec2 GetMax() const;

    bool IsColliding(const ColliderComponent& colliderComponent) const;
    bool TestCircleCircle(const ColliderComponent& colliderComponent) const;
    bool TestBoxBox(const ColliderComponent& colliderComponent) const;
    bool TestCircleBox(const ColliderComponent& colliderComponent) const;
    bool TestBoxCircle(const ColliderComponent& colliderComponent) const;

    // Networked
    glm::vec2 m_size;
    F32 m_radius;
    ShapeType m_shapeType;

    // Local
    glm::vec2 m_position;
};
}

#endif
