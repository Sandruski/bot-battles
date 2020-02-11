#include "TransformComponent.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "Input.h"
#include "InputComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void TransformComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, ReplicationActionType replicationActionType, Entity entity)
{
    assert(replicationActionType == ReplicationActionType::CREATE || replicationActionType == ReplicationActionType::UPDATE);

    Vec3 newPosition;
    F32 newRotation = 0.0f;
    const bool hasPosition = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION);
    if (hasPosition) {
        inputStream.Read(newPosition);
    }
    const bool hasRotation = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION);
    if (hasRotation) {
        inputStream.Read(newRotation);
    }

    if (replicationActionType == ReplicationActionType::CREATE) {
        if (hasPosition) {
            m_position = newPosition;
        }
        if (hasRotation) {
            m_rotation = newRotation;
        }
    } else {
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
        if (isLocalPlayer) {
            if (clientComponent.m_isServerReconciliation) {
                Replay(hasPosition, hasRotation, newPosition, newRotation);
            } else {
                if (hasPosition) {
                    m_position = newPosition;
                }
                if (hasRotation) {
                    m_rotation = newRotation;
                }
            }
        } else {
            if (clientComponent.m_isEntityInterpolation) {
                if (hasPosition || hasRotation)
                {
                    if (!hasPosition) {
                        if (!m_transformBuffer.IsEmpty()) {
                            newPosition = m_transformBuffer.GetLast().m_position;
                        }
                        else {
                            newPosition = m_position;
                        }
                    }
                    if (!hasRotation) {
                        if (!m_transformBuffer.IsEmpty()) {
                            newRotation = m_transformBuffer.GetLast().m_rotation;
                        }
                        else {
                            newRotation = m_rotation;
                        }
                    }
                    F32 startFrameTime = Time::GetInstance().GetStartFrameTime();
                    Transform transform = Transform(newPosition, newRotation, startFrameTime);
                    m_transformBuffer.Add(transform);
                    ILOG("CLIENT Has changed. Now pos x is: %f", newPosition.x);
                    ILOG("CLIENT INSERT NUMBER %u", m_transformBuffer.m_back - 1);
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
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::Replay(bool updatePosition, bool updateRotation, Vec3 newPosition, F32 newRotation)
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    Transform& firstTransform = m_transformBuffer.Get(clientComponent.m_lastAckdFrame);
    Vec3& position = firstTransform.m_position;
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

        U32 front = clientComponent.m_inputBuffer.m_front;
        assert(front == m_transformBuffer.m_front);
        U32 back = clientComponent.m_inputBuffer.m_back;
        assert(back == m_transformBuffer.m_back);

        for (U32 i = clientComponent.m_lastAckdFrame; i < back; ++i) {
            Transform& transform = m_transformBuffer.Get(i);
            const Input& input = clientComponent.m_inputBuffer.Get(i);
            const InputComponent& inputComponent = input.GetInputComponent();
            F32 dt = input.GetDt();

            // TODO: use TransformComponent's (static?) UpdatePosition and UpdateRotation methods
            if (replayPosition) {
                transform.m_position.x += inputComponent.m_acceleration.x * dt;
                transform.m_position.y += inputComponent.m_acceleration.y * dt;
            }
            if (replayRotation) {
                transform.m_rotation += inputComponent.m_angularAcceleration * dt;
            }
        }

        Transform& lastTransform = m_transformBuffer.GetLast();
        if (replayPosition) {
            m_position = lastTransform.m_position;
        }
        if (replayRotation) {
            m_rotation = lastTransform.m_rotation;
        }
    }

    m_transformBuffer.Remove(clientComponent.m_lastAckdFrame);
    clientComponent.m_inputBuffer.Remove(clientComponent.m_lastAckdFrame);
}
}
