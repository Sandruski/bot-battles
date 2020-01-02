#include "..\shared\TransformComponent.h"
#include "TransformComponent.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "MoveComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void TransformComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, ReplicationActionType replicationActionType, Entity entity)
{
    assert(replicationActionType == ReplicationActionType::CREATE || replicationActionType == ReplicationActionType::UPDATE);

    Vec3 oldPosition = m_position;
    F32 oldRotation = m_rotation;

    const bool hasPosition = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION);
    if (hasPosition) {
        inputStream.Read(m_position);
    }
    const bool hasRotation = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION);
    if (hasRotation) {
        inputStream.Read(m_rotation);
    }

    if (hasPosition || hasRotation) {
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
        if (isLocalPlayer) {
            ClientSidePredictionForLocalPlayer(hasPosition, hasRotation);
            if (replicationActionType != ReplicationActionType::CREATE) {
                ClientSideInterpolation(oldPosition, oldRotation);
            }
        } else {
            ClientSidePredictionForRemotePlayer(entity);
            if (replicationActionType != ReplicationActionType::CREATE) {
                ClientSideInterpolation(oldPosition, oldRotation);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
// Move replay
void TransformComponent::ClientSidePredictionForLocalPlayer(bool updatePosition, bool updateRotation)
{
    MoveComponent& moveComponent = g_gameClient->GetMoveComponent();
    U32 moveCount = moveComponent.m_moves.GetMoveCount();
    for (U32 i = 0; i < moveCount; ++i) {

        const Move& move = moveComponent.m_moves.GetMove(i);
        const InputComponent& moveInputComponent = move.GetInput();
        F32 dt = move.GetDt();
        if (updatePosition) {
            UpdatePosition(moveInputComponent.m_acceleration, dt);
        }
        if (updateRotation) {
            UpdateRotation(moveInputComponent.m_angularAcceleration, dt);
        }
    }
}

//----------------------------------------------------------------------------------------------------
// Dead reckoning
void TransformComponent::ClientSidePredictionForRemotePlayer(Entity entity)
{
    F32 dt = Time::GetInstance().GetDt();
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    F32 rtt = clientComponent.m_RTT;
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetComponentManager().GetComponent<InputComponent>(entity);

    while (rtt >= dt) {

        UpdatePosition(inputComponent.lock()->m_acceleration, dt);
        UpdateRotation(inputComponent.lock()->m_angularAcceleration, dt);
        // TODO: should rotation also be here?

        rtt -= dt;
    }
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::ClientSideInterpolation(const Vec3& oldPosition, F32 oldRotation)
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    F32 rtt = clientComponent.m_RTT;
    F32 startFrameTime = Time::GetInstance().GetStartFrameTime();

    if (oldPosition != m_position) {
        if (m_outOfSyncTimestamp == 0.0f) {
            m_outOfSyncTimestamp = startFrameTime;
        }

        F32 outOfSyncTime = startFrameTime - m_outOfSyncTimestamp;
        if (outOfSyncTime < rtt) {
            F32 t = outOfSyncTime / rtt;
            Vec3 newPosition = Lerp(oldPosition, m_position, t);
            m_position = newPosition;
        }
    } else {
        m_outOfSyncTimestamp = 0.0f;
    }

    if (oldRotation != m_rotation) {
    }
}
}
