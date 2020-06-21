#include "TransformComponent.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "Input.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "ReplicationManagerClient.h"
#include "RigidbodyComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void TransformComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity)
{
    U64 characterDirtyState = 0;

    glm::vec2 oldPosition = m_position;
    glm::vec2 newPosition = glm::vec2(0.0f, 0.0f);
    const bool hasPosition = dirtyState & static_cast<U64>(ComponentMemberType::TRANSFORM_POSITION);
    if (hasPosition) {
        inputStream.Read(newPosition);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::TRANSFORM_LAYER_TYPE)) {
        inputStream.Read(m_layerType);
        characterDirtyState |= static_cast<U64>(ComponentMemberType::TRANSFORM_LAYER_TYPE);
    }
    F32 oldRotation = m_rotation;
    F32 newRotation = 0.0f;
    const bool hasRotation = dirtyState & static_cast<U64>(ComponentMemberType::TRANSFORM_ROTATION);
    if (hasRotation) {
        inputStream.Read(newRotation);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::TRANSFORM_SCALE)) {
        inputStream.Read(m_scale);
        characterDirtyState |= static_cast<U64>(ComponentMemberType::TRANSFORM_SCALE);
    }

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    const bool isLocalEntity = clientComponent.lock()->IsLocalEntity(entity);
    if (isLocalEntity) {
        if (replicationActionType == ReplicationActionType::CREATE
            || !clientComponent.lock()->m_isServerReconciliation) {
            if (hasPosition) {
                m_position = newPosition;
            }
            if (hasRotation) {
                m_rotation = newRotation;
            }

            std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameClient->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
            if (!rigidbodyComponent.expired()) {
                rigidbodyComponent.lock()->m_body->SetTransform(b2Vec2(PIXELS_TO_METERS(m_position.x), PIXELS_TO_METERS(m_position.y)), glm::radians(m_rotation));

                b2Vec2 physicsPosition = rigidbodyComponent.lock()->m_body->GetPosition();
                m_position = glm::vec2(METERS_TO_PIXELS(physicsPosition.x), METERS_TO_PIXELS(physicsPosition.y));
                float32 physicsRotation = rigidbodyComponent.lock()->m_body->GetAngle();
                m_rotation = glm::degrees(physicsRotation);
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

            Replay(hasPosition, hasRotation, newPosition, newRotation, entity);
        }
    } else {
        if (clientComponent.lock()->m_isEntityInterpolation) {
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

                    b2Vec2 physicsPosition = rigidbodyComponent.lock()->m_body->GetPosition();
                    m_position = glm::vec2(METERS_TO_PIXELS(physicsPosition.x), METERS_TO_PIXELS(physicsPosition.y));
                    float32 physicsRotation = rigidbodyComponent.lock()->m_body->GetAngle();
                    m_rotation = glm::degrees(physicsRotation);
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

                b2Vec2 physicsPosition = rigidbodyComponent.lock()->m_body->GetPosition();
                m_position = glm::vec2(METERS_TO_PIXELS(physicsPosition.x), METERS_TO_PIXELS(physicsPosition.y));
                float32 physicsRotation = rigidbodyComponent.lock()->m_body->GetAngle();
                m_rotation = glm::degrees(physicsRotation);
            }
        }
    }

    if (oldPosition != m_position || oldRotation != m_rotation) {
        characterDirtyState |= static_cast<U64>(ComponentMemberType::TRANSFORM_POSITION) | static_cast<U64>(ComponentMemberType::TRANSFORM_ROTATION);
    }

    if (characterDirtyState > 0) {
        Event newComponentEvent;
        newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
        newComponentEvent.component.dirtyState = characterDirtyState;
        newComponentEvent.component.entity = entity;
        clientComponent.lock()->m_replicationManager.NotifyEvent(newComponentEvent);
    }
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::Replay(bool updatePosition, bool updateRotation, const glm::vec2& newPosition, F32 newRotation, Entity entity)
{
    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (m_inputTransformBuffer.IsEmpty() || clientComponent.lock()->m_inputBuffer.IsEmpty()) {
        return;
    }

    std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameClient->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
    if (rigidbodyComponent.expired()) {
        return;
    }

    U32 index = m_inputTransformBuffer.m_front;
    bool isFound = false;
    while (index < m_inputTransformBuffer.m_back) {
        const Transform& transform = m_inputTransformBuffer.Get(index);
        if (transform.GetFrame() == clientComponent.lock()->m_lastAckdFrame) {
            isFound = true;
            break;
        }
        ++index;
    }
    if (!isFound) {
        return;
    }

    std::weak_ptr<PhysicsComponent> physicsComponent = g_gameClient->GetPhysicsComponent();

    Transform& firstTransform = m_inputTransformBuffer.Get(index);
    glm::vec2& position = firstTransform.m_position;
    F32& rotation = firstTransform.m_rotation;

    bool replayPosition = false;
    if (updatePosition) {
        glm::vec2 minNewPosition = newPosition - physicsComponent.lock()->m_epsilon;
        glm::vec2 maxNewPosition = newPosition + physicsComponent.lock()->m_epsilon;
        if ((position.x < minNewPosition.x || position.y < minNewPosition.y)
            || (position.x > maxNewPosition.x || position.y > maxNewPosition.y)) {
            replayPosition = true;
        }
    }

    bool replayRotation = false;
    if (updateRotation) {
        F32 minNewRotation = newRotation - physicsComponent.lock()->m_epsilon;
        F32 maxNewRotation = newRotation + physicsComponent.lock()->m_epsilon;
        if (rotation < minNewRotation
            || rotation > maxNewRotation) {
            replayRotation = true;
        }
    }

    if (replayPosition || replayRotation) {
        ILOG("REPLAY");
        ILOG("Current vs new positions at frame %u is %f %f and %f %f", clientComponent.lock()->m_lastAckdFrame, position.x, position.y, newPosition.x, newPosition.y);

        position = newPosition;
        rotation = newRotation;
        rigidbodyComponent.lock()->m_body->SetTransform(b2Vec2(PIXELS_TO_METERS(position.x), PIXELS_TO_METERS(position.y)), glm::radians(rotation));

        ILOG("We loop %u transforms", clientComponent.lock()->m_inputBuffer.m_back - index - 1);
        for (U32 i = index + 1; i < clientComponent.lock()->m_inputBuffer.m_back; ++i) {
            const Input& input = clientComponent.lock()->m_inputBuffer.Get(i);
            const InputComponent& inputComponent = input.GetInputComponent();
            U64 dirtyState = input.GetDirtyState();

            glm::vec2 linearVelocity = glm::vec2(0.0f, 0.0f);
            F32 angularVelocity = 0.0f;

            const bool hasLinearVelocity = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
            if (hasLinearVelocity) {
                linearVelocity += inputComponent.m_linearVelocity;
            }
            const bool hasAngularVelocity = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
            if (hasAngularVelocity) {
                angularVelocity += inputComponent.m_angularVelocity;
            }

            if (!std::isfinite(linearVelocity.x)) {
                linearVelocity.x = 0.0f;
            }

            if (!std::isfinite(linearVelocity.y)) {
                linearVelocity.y = 0.0f;
            }

            if (!std::isfinite(angularVelocity)) {
                angularVelocity = 0.0f;
            }

            F32 linearVelocityLength = glm::length(linearVelocity);
            if (linearVelocityLength > rigidbodyComponent.lock()->m_maxLinearVelocity) {
                if (glm::length(linearVelocity) > 0.0f) {
                    linearVelocity = glm::normalize(linearVelocity);
                }
                linearVelocity *= rigidbodyComponent.lock()->m_maxLinearVelocity;
            }

            glm::clamp(angularVelocity, -rigidbodyComponent.lock()->m_maxAngularVelocity, rigidbodyComponent.lock()->m_maxAngularVelocity);

            rigidbodyComponent.lock()->m_body->SetLinearVelocity(b2Vec2(PIXELS_TO_METERS(linearVelocity.x), PIXELS_TO_METERS(linearVelocity.y)));
            rigidbodyComponent.lock()->m_body->SetAngularVelocity(glm::radians(angularVelocity));

            rigidbodyComponent.lock()->m_body->SetActive(true);

            physicsComponent.lock()->Step();

            Transform& transform = m_inputTransformBuffer.Get(i);
            if (replayPosition) {
                b2Vec2 physicsPosition = rigidbodyComponent.lock()->m_body->GetPosition();
                ILOG("Position at frame %u %u was %f %f and now is %f %f", input.GetFrame(), transform.GetFrame(), transform.m_position.x, transform.m_position.y, METERS_TO_PIXELS(physicsPosition.x), METERS_TO_PIXELS(physicsPosition.y));
                transform.m_position = glm::vec2(METERS_TO_PIXELS(physicsPosition.x), METERS_TO_PIXELS(physicsPosition.y));
            }
            if (replayRotation) {
                float32 physicsRotation = rigidbodyComponent.lock()->m_body->GetAngle();
                transform.m_rotation = glm::degrees(physicsRotation);
            }

            rigidbodyComponent.lock()->m_body->SetActive(false);
        }

        Transform& lastTransform = m_inputTransformBuffer.GetLast();
        ILOG("My position is %f %f compared to %f %f", m_position.x, m_position.y, lastTransform.m_position.x, lastTransform.m_position.y);
        m_position = lastTransform.m_position;
        ILOG("My rotation is %f compared to %f", m_rotation, lastTransform.m_rotation);
        m_rotation = lastTransform.m_rotation;
    }
}
}
