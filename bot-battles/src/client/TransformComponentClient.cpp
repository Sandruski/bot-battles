#include "TransformComponent.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "Input.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void TransformComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ENABLED)) {
        inputStream.Read(m_isEnabled);
    }
    glm::vec2 newPosition = glm::vec2(0.0f, 0.0f);
    const bool hasPosition = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION);
    if (hasPosition) {
        inputStream.Read(newPosition);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_LAYER_TYPE)) {
        inputStream.Read(m_layerType);
    }
    F32 newRotation = 0.0f;
    const bool hasRotation = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION);
    if (hasRotation) {
        inputStream.Read(newRotation);
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    const bool isLocalEntity = clientComponent.IsLocalEntity(entity);
    if (isLocalEntity) {
        if (replicationActionType == ReplicationActionType::CREATE
            || !clientComponent.m_isServerReconciliation) {
            if (hasPosition) {
                m_position = newPosition;
            }
            if (hasRotation) {
                m_rotation = newRotation;
            }
        } else {
            if (!hasPosition) {
                if (!m_inputTransformBuffer.IsEmpty()) {
                    newPosition = m_inputTransformBuffer.GetLast().m_position;
                } else {
                    newPosition = m_position;
                }
            }
            if (!hasRotation) {
                if (!m_inputTransformBuffer.IsEmpty()) {
                    newRotation = m_inputTransformBuffer.GetLast().m_rotation;
                } else {
                    newRotation = m_rotation;
                }
            }

            Replay(hasPosition, hasRotation, newPosition, newRotation);
        }
    } else {
        if (clientComponent.m_isEntityInterpolation) {
            if (replicationActionType == ReplicationActionType::CREATE) {
                if (hasPosition) {
                    m_position = newPosition;
                }
                if (hasRotation) {
                    m_rotation = newRotation;
                }
            } else {
                if (!hasPosition) {
                    if (!m_transformBuffer.IsEmpty()) {
                        newPosition = m_transformBuffer.GetLast().m_position;
                    } else {
                        newPosition = m_position;
                    }
                }
                if (!hasRotation) {
                    if (!m_transformBuffer.IsEmpty()) {
                        newRotation = m_transformBuffer.GetLast().m_rotation;
                    } else {
                        newRotation = m_rotation;
                    }
                }
            }

            if (hasPosition || hasRotation) {
                Transform transform = Transform(newPosition, newRotation, frame);
                m_transformBuffer.Add(transform);
                ILOG("Added frame %u", frame);
            }
        } else {
            if (hasPosition) {
                m_position = newPosition;
            }
            if (hasRotation) {
                m_rotation = newRotation;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::Replay(bool updatePosition, bool updateRotation, glm::vec2 newPosition, F32 newRotation)
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (m_inputTransformBuffer.IsEmpty() || clientComponent.m_inputBuffer.IsEmpty()) {
        return;
    }

    U32 index = m_inputTransformBuffer.m_front;
    bool isFound = false;
    while (index < m_inputTransformBuffer.m_back) {
        const Transform& transform = m_inputTransformBuffer.Get(index);
        if (transform.GetFrame() == clientComponent.m_lastAckdFrame) {
            isFound = true;
            break;
        }
        ++index;
    }
    if (!isFound) {
        return;
    }

    Transform& firstTransform = m_inputTransformBuffer.Get(index);
    glm::vec2& position = firstTransform.m_position;
    F32& rotation = firstTransform.m_rotation;

    const bool replayPosition = updatePosition ? position != newPosition : false;
    const bool replayRotation = updateRotation ? rotation != newRotation : false;

    if (replayPosition || replayRotation) {
        ILOG("REPLAY");

        if (replayPosition) {
            position = newPosition;
        }
        if (replayRotation) {
            rotation = newRotation;
        }

        PhysicsComponent& physicsComponent = g_gameClient->GetPhysicsComponent();
        for (U32 i = clientComponent.m_inputBuffer.m_front; i < clientComponent.m_inputBuffer.m_back; ++i) {
            Transform& transform = m_inputTransformBuffer.Get(i);
            const Input& input = clientComponent.m_inputBuffer.Get(i);
            const InputComponent& inputComponent = input.GetInputComponent();

            if (replayPosition) {
                transform.m_position += inputComponent.m_acceleration * physicsComponent.m_timeStep;
            }
            if (replayRotation) {
                transform.m_rotation += inputComponent.m_angularAcceleration * physicsComponent.m_timeStep;
            }
        }

        Transform& lastTransform = m_inputTransformBuffer.GetLast();
        if (replayPosition) {
            m_position = lastTransform.m_position;
        }
        if (replayRotation) {
            m_rotation = lastTransform.m_rotation;
        }
    }
}
}
