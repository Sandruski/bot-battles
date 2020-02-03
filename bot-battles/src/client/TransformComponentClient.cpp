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

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();

    const bool hasPosition = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION);
    if (hasPosition) {
        inputStream.Read(m_toPosition);
    }
    const bool hasRotation = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION);
    if (hasRotation) {
        inputStream.Read(m_toRotation);
    }

    if (hasPosition || hasRotation) {
        if (replicationActionType == ReplicationActionType::CREATE) {
            if (hasPosition) {
                m_position = m_toPosition;
            }
            if (hasRotation) {
                m_rotation = m_toRotation;
            }
        } else {
            const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
            if (isLocalPlayer) {
                if (clientComponent.m_isServerReconciliation) {
                    Replay(hasPosition, hasRotation);
                } else {
                    if (hasPosition) {
                        m_position = m_toPosition;
                    }
                    if (hasRotation) {
                        m_rotation = m_toRotation;
                    }
                }
            } else {
                if (clientComponent.m_isEntityInterpolation) {
                    Interpolate(hasPosition, hasRotation);
                } else {
                    if (hasPosition) {
                        m_position = m_toPosition;
                    }
                    if (hasRotation) {
                        m_rotation = m_toRotation;
                    }
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::Replay(bool updatePosition, bool updateRotation)
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    Transform& firstTransform = m_transformBuffer.Get(clientComponent.m_lastAckdFrame);
    Vec3& position = firstTransform.m_position;
    F32& rotation = firstTransform.m_rotation;

    const bool replayPosition = updatePosition ? position != m_toPosition : false;
    const bool replayRotation = updateRotation ? rotation != m_toRotation : false;

    if (replayPosition || replayRotation) {
        if (replayPosition) {
            position = m_toPosition;
        }
        if (replayRotation) {
            rotation = m_toRotation;
        }

        U32 front = clientComponent.m_inputBuffer.m_front;
        assert(front == clientComponent.m_lastAckdFrame);
        U32 back = clientComponent.m_inputBuffer.m_back;
        assert(front == m_transformBuffer.m_back);
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
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::Interpolate(bool updatePosition, bool updateRotation)
{
    const bool interpolatePosition = updatePosition ? m_position != m_toPosition : false;
    const bool interpolateRotation = updateRotation ? m_rotation != m_toRotation : false;

    if (interpolatePosition || interpolateRotation) {
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        F32 time = Time::GetInstance().GetTime();

        if (interpolatePosition) {
            m_fromPosition = m_position;
            m_positionOutOfSyncTimestamp = time;
            //m_fromPositionFrame = clientComponent.m_lastAckdFrame; // TODO
            m_toPositionFrame = clientComponent.m_lastAckdFrame;
        } else {
            m_positionOutOfSyncTimestamp = 0.0f;
        }
        if (interpolateRotation) {
            m_fromRotation = m_rotation;
            m_rotationOutOfSyncTimestamp = time;
        } else {
            m_rotationOutOfSyncTimestamp = 0.0f;
        }
    }
}
}
