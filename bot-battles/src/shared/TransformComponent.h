#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif _SERVER
#include "NetworkableWriteObject.h"
#endif

namespace sand {

struct InputComponent;

//----------------------------------------------------------------------------------------------------
// Entity Component
struct TransformComponent : public Component,
#ifdef _CLIENT
                            public NetworkableReadObject
#elif _SERVER
                            public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::TRANSFORM; }

    TransformComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, ReplicationActionType replicationActionType, Entity entity) override;
    void ClientSidePredictionForLocalPlayer(bool updatePosition, bool updateRotation);
    void ClientSidePredictionForRemotePlayer(Entity entity);
    void ClientSideInterpolation(const Vec3& oldPosition, F32 oldRotation);
#elif _SERVER
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    void UpdatePosition(const Vec2& acceleration, F32 dt);
    void UpdateRotation(F32 angularAcceleration, F32 dt);

    Vec3 m_position;
    F32 m_rotation;
#ifdef _CLIENT
    F32 m_outOfSyncTimestamp;
#endif
};
}

#endif
