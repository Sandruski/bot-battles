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

    //Vec3 oldPosition = m_position;
    //F32 oldRotation = m_rotation;

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
        }
        else {
            ClientComponent& clientComponent = g_gameClient->GetClientComponent();
            const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
            if (isLocalPlayer) {
                if (clientComponent.m_isServerReconciliation) {
                    if (clientComponent.m_isFrameDirty) {
                        Replay(clientComponent, hasPosition, hasRotation);
                    }
                }
                else {
                    if (hasPosition) {
                        m_position = m_toPosition;
                    }
                    if (hasRotation) {
                        m_rotation = m_toRotation;
                    }
                }
            }
            else {
                if (clientComponent.m_isEntityInterpolation) {
                    if (m_position != m_endPosition) {
                        m_startPosition = m_position;
                        m_outOfSyncTimestamp = Time::GetInstance().GetTime();
                        ILOG("BECAME OUT OF SYNC %f", m_outOfSyncTimestamp);
                    }
                    else {
                        m_outOfSyncTimestamp = 0.0f;
                    }
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::Replay(ClientComponent& clientComponent, bool updatePosition, bool updateRotation)
{
    Vec3 position = clientComponent.m_transformBuffer.Get(clientComponent.m_frame).m_position;
    clientComponent.m_transformBuffer.Remove(clientComponent.m_frame);
    clientComponent.m_isFrameDirty = false;

    if (position != m_toPosition) {
        position = m_toPosition;

        U32 front = clientComponent.m_inputBuffer.m_front;
        assert(front == clientComponent.m_transformBuffer.m_front);
        U32 back = clientComponent.m_inputBuffer.m_back;
        assert(front == clientComponent.m_transformBuffer.m_back);

        for (U32 i = front; i < back; ++i) {
            TransformComponent& transformComponent = clientComponent.m_transformBuffer.Get(i);
            const Input& input = clientComponent.m_inputBuffer.Get(i);
            const InputComponent& inputComponent = input.GetInputComponent();
            F32 dt = input.GetDt();

            if (updatePosition) {
                transformComponent.UpdatePosition(inputComponent.m_acceleration, dt);
            }
            if (updateRotation) {
                transformComponent.UpdateRotation(inputComponent.m_angularAcceleration, dt);
            }
            ILOG("REPLAY MOVE %u", input.GetFrame());
        }

        m_position = clientComponent.m_transformBuffer.GetLast().m_position;
    }
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::Interpolate(ClientComponent& clientComponent, const Vec3& oldPosition, F32 oldRotation)
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    F32 rtt = clientComponent.m_RTT;
    F32 time = Time::GetInstance().GetTime();

    if (oldPosition != m_position) {
        if (m_outOfSyncTimestamp == 0.0f) {
            m_outOfSyncTimestamp = time;
        }

        F32 outOfSyncTime = time - m_outOfSyncTimestamp;
        if (outOfSyncTime < rtt) {
            F32 t = outOfSyncTime / rtt;
            m_position = Lerp(oldPosition, m_position, t);
        }
    } else {
        m_outOfSyncTimestamp = 0.0f;
    }

    if (oldRotation != m_rotation) {
        // TODO
    }
}
}
