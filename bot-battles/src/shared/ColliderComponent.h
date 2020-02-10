#ifndef __COLLIDER_COMPONENT_H__
#define __COLLIDER_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

struct TransformComponent;

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

    ColliderComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    SDL_Rect GetRect() const;

    Vec2 m_position;
    Vec2 m_size;
};
}

#endif
