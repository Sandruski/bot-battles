#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif
#include "Transform.h"

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
    void Read(InputMemoryStream& inputStream, U32 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
    void Replay(bool updatePosition, bool updateRotation, Vec3 newPosition, F32 newRotation);
#elif defined(_SERVER)
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    void UpdateTransform(const Vec2& acceleration, F32 angularAcceleration, F32 dt);
    void UpdatePosition(const Vec2& acceleration, F32 dt);
    void UpdateRotation(F32 angularAcceleration, F32 dt);

    Vec2 GetPosition() const;
    Vec2 GetRotation() const;

    Vec3 m_position;
    F32 m_rotation;
    //Vec2 m_velocity;
    //F32 m_angularVelocity;
    CircularBuffer<Transform, MAX_FRAMES> m_transformBuffer;
    CircularBuffer<Transform, MAX_FRAMES> m_inputTransformBuffer;
};
}

#endif
