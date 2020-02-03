#include "NavigationSystemClient.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
NavigationSystemClient::NavigationSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
}

//----------------------------------------------------------------------------------------------------
bool NavigationSystemClient::Update()
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    F32 time = Time::GetInstance().GetTime();

    for (auto& entity : m_entities) {

        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) == INVALID_NETWORK_ID) {
            continue;
        }

        const bool isLocalPlayer = clientComponent.IsLocalPlayer(entity);
        if (isLocalPlayer) {
            if (clientComponent.m_isClientSidePrediction) {
                if (clientComponent.m_isLastMovePending) {
                    const Input& input = clientComponent.m_inputBuffer.GetLast();
                    const InputComponent& inputComponent = input.GetInputComponent();
                    F32 dt = input.GetDt();

                    std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
                    transformComponent.lock()->UpdateTransform(inputComponent.m_acceleration, inputComponent.m_angularAcceleration, dt);
                    Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, 0);
                    transformComponent.lock()->m_transformBuffer.Add(transform);

                    clientComponent.m_isLastMovePending = false;
                }
            }
        } else {
            if (clientComponent.m_isEntityInterpolation) {
                std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
                if (transformComponent.lock()->m_position != transformComponent.lock()->m_toPosition) {
                    F32 outOfSyncTime = time - transformComponent.lock()->m_positionOutOfSyncTimestamp; // TODO: pick frame start time
                    F32 t = outOfSyncTime / ENTITY_INTERPOLATION_PERIOD;
                    transformComponent.lock()->m_position = Lerp(transformComponent.lock()->m_fromPosition, transformComponent.lock()->m_toPosition, t <= 1.0f ? t : 1.0f);
                } else {
                    transformComponent.lock()->m_positionOutOfSyncTimestamp = 0.0f;
                }
                if (transformComponent.lock()->m_rotation != transformComponent.lock()->m_toRotation) {
                    F32 outOfSyncTime = time - transformComponent.lock()->m_rotationOutOfSyncTimestamp; // TODO: pick frame start time
                    F32 t = outOfSyncTime / ENTITY_INTERPOLATION_PERIOD;
                    transformComponent.lock()->m_rotation = Lerp(transformComponent.lock()->m_fromRotation, transformComponent.lock()->m_toRotation, t <= 1.0f ? t : 1.0f);
                } else {
                    transformComponent.lock()->m_rotationOutOfSyncTimestamp = 0.0f;
                }
            }
        }
    }

    return true;
}
}
