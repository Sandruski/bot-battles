#include "TransformComponent.h"

#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "MoveComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void TransformComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, bool /*isLocalPlayer*/)
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
    }
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::ClientPredictionForLocalPlayer()
{
    //MoveComponent& moveComponent = g_gameClient->GetMoveComponent();
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::ClientPredictionForRemotePlayer()
{
}
}
