#include "TransformComponent.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "Input.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "RigidbodyComponent.h"

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

            std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameClient->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
            if (!rigidbodyComponent.expired()) {
                rigidbodyComponent.lock()->m_body->SetTransform(b2Vec2(PIXELS_TO_METERS(m_position.x), PIXELS_TO_METERS(m_position.y)), glm::radians(m_rotation));
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

                std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameClient->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
                if (!rigidbodyComponent.expired()) {
                    rigidbodyComponent.lock()->m_body->SetTransform(b2Vec2(PIXELS_TO_METERS(m_position.x), PIXELS_TO_METERS(m_position.y)), glm::radians(m_rotation));
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
            }
        } else {
            if (hasPosition) {
                m_position = newPosition;
            }
            if (hasRotation) {
                m_rotation = newRotation;
            }

            std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameClient->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
            if (!rigidbodyComponent.expired()) {
                rigidbodyComponent.lock()->m_body->SetTransform(b2Vec2(PIXELS_TO_METERS(m_position.x), PIXELS_TO_METERS(m_position.y)), glm::radians(m_rotation));
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

    PhysicsComponent& physicsComponent = g_gameClient->GetPhysicsComponent();

    Transform& firstTransform = m_inputTransformBuffer.Get(index);
    glm::vec2& position = firstTransform.m_position;
    F32& rotation = firstTransform.m_rotation;

    bool replayPosition = false;
    if (updatePosition) {
        glm::vec2 minNewPosition = newPosition - physicsComponent.m_epsilon;
        glm::vec2 maxNewPosition = newPosition + physicsComponent.m_epsilon;
        if ((position.x < minNewPosition.x || position.y < minNewPosition.y)
            || (position.x > maxNewPosition.x || position.y > maxNewPosition.y)) {
            replayPosition = true;
        }
    }

    bool replayRotation = false;
    if (updateRotation) {
        F32 minNewRotation = newRotation - physicsComponent.m_epsilon;
        F32 maxNewRotation = newRotation + physicsComponent.m_epsilon;
        if (rotation < minNewRotation
            || rotation > maxNewRotation) {
            replayPosition = true;
        }
    }

    if (replayPosition || replayRotation) {
        assert(false); // TODO: remove this once the networking is finished :)
        ILOG("REPLAY");
        ILOG("Current vs new positions at frame %u is %f %f and %f %f", clientComponent.m_lastAckdFrame, position.x, position.y, newPosition.x, newPosition.y);

        if (replayPosition) {
            position = newPosition;
        }
        if (replayRotation) {
            rotation = newRotation;
        }

        for (U32 i = clientComponent.m_inputBuffer.m_front; i < clientComponent.m_inputBuffer.m_back; ++i) {
            const Input& input = clientComponent.m_inputBuffer.Get(i);
            const InputComponent& inputComponent = input.GetInputComponent();

            Transform& transform = m_inputTransformBuffer.Get(i);

            if (replayPosition) {
                transform.m_position += inputComponent.m_linearVelocity * physicsComponent.m_timeStep;
            }
            if (replayRotation) {
                transform.m_rotation += inputComponent.m_angularVelocity * physicsComponent.m_timeStep;
            }
        }

        // TODO: move towards m_position and m_rotation with LINEAR_VELOCITY and ANGULAR_VELOCITY from current m_position and m_rotation?
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
