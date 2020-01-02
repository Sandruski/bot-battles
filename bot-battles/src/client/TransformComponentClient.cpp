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
    //Vec3 oldPosition = m_position;
    //F32 oldRotation = m_rotation;

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
            ClientPredictionForLocalPlayer(hasPosition, hasRotation);
        } else {
            ClientPredictionForRemotePlayer(entity);
        }
    }
}

//----------------------------------------------------------------------------------------------------
// Move replay
void TransformComponent::ClientPredictionForLocalPlayer(bool updatePosition, bool updateRotation)
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
void TransformComponent::ClientPredictionForRemotePlayer(Entity entity)
{
    F32 dt = Time::GetInstance().GetDt();
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    F32 rtt = clientComponent.m_RTT;
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetComponentManager().GetComponent<InputComponent>(entity);

    while (rtt >= dt) {

        UpdatePosition(inputComponent.lock()->m_acceleration, dt);
        // TODO: also UpdateRotation?

        rtt -= dt;
    }
}
}
