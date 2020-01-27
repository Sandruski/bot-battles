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

    ILOG("POS BEFORE %f %f", m_position.x, m_position.y);

    const bool hasPosition = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION);
    if (hasPosition) {
        inputStream.Read(m_lastPosition);
    }
    const bool hasRotation = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION);
    if (hasRotation) {
        inputStream.Read(m_rotation);
    }

    ILOG("POS READ %f %f", m_position.x, m_position.y);

    // TODO: read velocity and angular velocity

    if (hasPosition || hasRotation) {
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
        if (isLocalPlayer) {
            m_position = m_lastPosition;
            if (clientComponent.m_isServerReconciliation) {
                ClientSidePrediction(hasPosition, hasRotation);
                /*
                if (replicationActionType != ReplicationActionType::CREATE) {
                    ClientSideInterpolation(oldPosition, oldRotation);
                }*/
            }
        } else {
            if (clientComponent.m_isEntityInterpolation) {
                if (replicationActionType == ReplicationActionType::CREATE) {
                    m_position = m_lastPosition;
                } else {
                    if (m_position != m_lastPosition) {
                        if (m_outOfSyncTimestamp == 0.0f) {
                            m_outOfSyncTimestamp = Time::GetInstance().GetTime();
                        }
                    } else {
                        m_outOfSyncTimestamp = 0.0f;
                    }
                }
            } else {
                m_position = m_lastPosition;
            }
            /*
            ClientSidePredictionForRemotePlayer(entity);
            if (replicationActionType != ReplicationActionType::CREATE) {
                ClientSideInterpolation(oldPosition, oldRotation);
            }*/
        }
    }

    ILOG("POS AFTER %f %f", m_position.x, m_position.y);
}

//----------------------------------------------------------------------------------------------------
// Move replay
void TransformComponent::ClientSidePrediction(bool updatePosition, bool updateRotation)
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    U32 moveCount = clientComponent.m_moves.GetMoveCount();
    ILOG("CLIENT SIDE PREDICTION FOR %u MOVES", moveCount);
    for (U32 i = 0; i < moveCount; ++i) {

        const Move& move = clientComponent.m_moves.GetMove(i);
        const InputComponent& moveInputComponent = move.GetInputComponent();
        F32 dt = move.GetDt();
        if (updatePosition) {
            UpdatePosition(moveInputComponent.m_acceleration, dt);
        }
        if (updateRotation) {
            UpdateRotation(moveInputComponent.m_angularAcceleration, dt);
        }
        ILOG("REPLAY MOVE %u", move.GetFrame());
    }
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
