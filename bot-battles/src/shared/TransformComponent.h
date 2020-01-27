#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

struct InputComponent;

//----------------------------------------------------------------------------------------------------
// Entity Component
struct TransformComponent : public Component
#ifdef _CLIENT
    ,
                            public NetworkableReadObject
#elif defined(_SERVER)
    ,
                            public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::TRANSFORM; }

    TransformComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, ReplicationActionType replicationActionType, Entity entity) override;
    void ClientSidePrediction(bool updatePosition, bool updateRotation);
    void ClientSideInterpolation(const Vec3& oldPosition, F32 oldRotation);
#elif defined(_SERVER)
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    void UpdateTransform(const Vec2& acceleration, F32 angularAcceleration, F32 dt);
    void UpdatePosition(const Vec2& acceleration, F32 dt);
    void UpdateRotation(F32 angularAcceleration, F32 dt);

    Vec3 m_startPosition;
    Vec3 m_endPosition;
    Vec3 m_position;
    F32 m_rotation;
    Vec2 m_velocity;
    F32 m_angularVelocity;
#ifdef _CLIENT
    F32 m_outOfSyncTimestamp;
#endif
};
}

#endif
