#include "TransformComponent.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void TransformComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, ReplicationActionType replicationActionType, Entity entity)
{
    assert(replicationActionType == ReplicationActionType::CREATE || replicationActionType == ReplicationActionType::UPDATE);

    //Vec3 oldPosition = m_position;
    //F32 oldRotation = m_rotation;

    ILOG("POS BEFORE %f %f", m_position.x);

    const bool hasPosition = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION);
    if (hasPosition) {
        inputStream.Read(m_endPosition);
    }
    const bool hasRotation = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION);
    if (hasRotation) {
        inputStream.Read(m_rotation);
    }

    ILOG("POS READ %f %f", m_endPosition.x);

    // TODO: read velocity and angular velocity

    if (hasPosition || hasRotation) {
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
        if (isLocalPlayer) {

            if (replicationActionType == ReplicationActionType::CREATE) {
                m_position = m_endPosition;
            } else if (clientComponent.m_isFrameDirty) { // TODO: remove this?
                U32 index = clientComponent.m_transformBuffer.GetIndex(clientComponent.m_frame);
                TransformComponent& transformComponent = clientComponent.m_transformBuffer.GetTransform(index);
                clientComponent.m_transformBuffer.Remove(clientComponent.m_frame);
                clientComponent.m_isFrameDirty = false;

                if (transformComponent.m_position != m_endPosition) {
                    transformComponent.m_position = m_endPosition;
                    //Replay(hasPosition, hasRotation);

                    U32 inputCount = clientComponent.m_inputBuffer.GetCount();
                    U32 transformCount = clientComponent.m_transformBuffer.GetCount();
                    assert(inputCount == transformCount);
                    ILOG("CLIENT SIDE PREDICTION FOR %u INPUTS", inputCount);
                    ILOG("Client Side Misprediction! Replaying inputs...");
                    for (U32 i = clientComponent.m_inputBuffer.m_front; i < clientComponent.m_inputBuffer.m_back; ++i) {

                        U32 index1 = clientComponent.m_transformBuffer.GetIndex(i);
                        transformComponent = clientComponent.m_transformBuffer.GetTransform(index1);

                        U32 index2 = clientComponent.m_inputBuffer.GetIndex(i);
                        const Input& input = clientComponent.m_inputBuffer.GetInput(index2);
                        const InputComponent& inputComponent = input.GetInputComponent();
                        F32 dt = input.GetDt();

                        if (hasPosition) {
                            transformComponent.UpdatePosition(inputComponent.m_acceleration, dt);
                        }
                        if (hasRotation) {
                            transformComponent.UpdateRotation(inputComponent.m_angularAcceleration, dt);
                        }
                        ILOG("REPLAY MOVE %u", input.GetFrame());
                    }
                    m_position = clientComponent.m_transformBuffer.GetLastTransform().m_position;
                }
            }
            //if (clientComponent.m_isServerReconciliation) {
            //ClientSidePrediction(hasPosition, hasRotation);
            /*
                if (replicationActionType != ReplicationActionType::CREATE) {
                    ClientSideInterpolation(oldPosition, oldRotation);
                }*/
            //}
        } else {
            if (clientComponent.m_isEntityInterpolation) {
                if (replicationActionType == ReplicationActionType::CREATE) {
                    m_position = m_endPosition;
                } else {
                    if (m_position != m_endPosition) {
                        m_startPosition = m_position;
                        m_outOfSyncTimestamp = Time::GetInstance().GetTime();
                        ILOG("BECAME OUT OF SYNC %f", m_outOfSyncTimestamp);
                    } else {
                        m_outOfSyncTimestamp = 0.0f;
                    }
                }
            } else {
                m_position = m_endPosition;
            }
            /*
            ClientSidePredictionForRemotePlayer(entity);
            if (replicationActionType != ReplicationActionType::CREATE) {
                ClientSideInterpolation(oldPosition, oldRotation);
            }*/
        }
    }

    ILOG("POS AFTER %f %f", m_position.x);
}

//----------------------------------------------------------------------------------------------------
// Move replay
void TransformComponent::Replay(bool /*updatePosition*/, bool /*updateRotation*/)
{
    /*
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    U32 moveCount = clientComponent.m_inputBuffer.GetCount();
    ILOG("CLIENT SIDE PREDICTION FOR %u MOVES", moveCount);
    for (U32 i = 0; i < moveCount; ++i) {

        const Input& input = clientComponent.m_inputBuffer.Get(i);
        const InputComponent& moveInputComponent = input.GetInputComponent();
        F32 dt = input.GetDt();
        if (updatePosition) {
            UpdatePosition(moveInputComponent.m_acceleration, dt);
        }
        if (updateRotation) {
            UpdateRotation(moveInputComponent.m_angularAcceleration, dt);
        }
        ILOG("REPLAY MOVE %u", input.GetFrame());
    }
    */
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::ClientSideInterpolation(const Vec3& oldPosition, F32 oldRotation)
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
