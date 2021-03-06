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
    void Read(InputMemoryStream& inputStream, U64 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
    void Replay(bool updatePosition, bool updateRotation, const glm::vec2& newPosition, F32 newRotation, Entity entity);
#elif defined(_SERVER)
    U64 Write(OutputMemoryStream& outputStream, U64 dirtyState) const override;
#endif

    glm::vec3 GetPositionAndLayer() const;
    glm::vec3 GetDebugPositionAndLayer() const;
    glm::vec2 GetDirection() const;

#ifdef _CLIENT
    // Python
    std::tuple<F32, F32> GetPyPosition() const
    {
        return std::make_tuple(m_position.x, m_position.y);
    }

    F32 GetPyRotation() const
    {
        return m_rotation;
    }

    std::tuple<F32, F32> GetPyDirection() const
    {
        glm::vec2 direction = GetDirection();
        return std::make_tuple(direction.x, direction.y);
    }
#endif

    // Networked
    glm::vec2 m_position;
    LayerType m_layerType;
    F32 m_rotation;
    F32 m_scale;

    // Local (client & server)
    CircularBuffer<Transform, MAX_TRANSFORMS> m_transformBuffer;
    /*
    Client: transform per frame received from server (for Entity Interpolation)
    -Add: ReplicationManagerClient and TransformComponentClient
    -Remove: RemotePlayerMovementSystem

    Server: transform per frame from server (for Server Rewind)
    -Add: MovementSystemServer
    -Remove: OutputSystemServer
    */
    CircularBuffer<Transform, MAX_TRANSFORMS> m_inputTransformBuffer;
    /*
    Client: transform per input from client (for Server Reconciliation)
    -Add: MovementSystemClient and WeaponSystemClient 
    -Remove: OutputSystemClient

    Server: transform per input received from client
    -Add: MovementSystemServer
    -Remove: OutputSystemServer
    */
};
}

#endif
