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
            ClientComponent& clientComponent = g_gameClient->GetClientComponent();
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
    TransformComponent& firstTransformComponent = clientComponent.m_transformBuffer.GetFirst();
    Vec3& position = firstTransformComponent.m_position;
    F32& rotation = firstTransformComponent.m_rotation;

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
        assert(front == clientComponent.m_transformBuffer.m_front);
        U32 back = clientComponent.m_inputBuffer.m_back;
        assert(front == clientComponent.m_transformBuffer.m_back);
        for (U32 i = front; i < back; ++i) {
            TransformComponent& transformComponent = clientComponent.m_transformBuffer.Get(i);
            const Input& input = clientComponent.m_inputBuffer.Get(i);
            const InputComponent& inputComponent = input.GetInputComponent();
            F32 dt = input.GetDt();

            if (replayPosition) {
                transformComponent.UpdatePosition(inputComponent.m_acceleration, dt);
            }
            if (replayRotation) {
                transformComponent.UpdateRotation(inputComponent.m_angularAcceleration, dt);
            }
        }

        TransformComponent& lastTransformComponent = clientComponent.m_transformBuffer.GetLast();
        if (replayPosition) {
            m_position = lastTransformComponent.m_position;
        }
        if (replayRotation) {
            m_rotation = lastTransformComponent.m_rotation;
        }
    }

    clientComponent.m_transformBuffer.RemoveFirst();
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::Interpolate(bool updatePosition, bool updateRotation)
{
    const bool interpolatePosition = updatePosition ? m_position != m_toPosition : false;
    const bool interpolateRotation = updateRotation ? m_rotation != m_toRotation : false;

    if (interpolatePosition || interpolateRotation) {
        F32 time = Time::GetInstance().GetTime();

        if (interpolatePosition) {
            m_fromPosition = m_position;
            m_positionOutOfSyncTimestamp = time;
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
