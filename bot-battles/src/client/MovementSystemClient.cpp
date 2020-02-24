#include "MovementSystemClient.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MovementSystemClient::MovementSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::LOCAL_PLAYER); // this should be input component
}

//----------------------------------------------------------------------------------------------------
bool MovementSystemClient::Update()
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    const bool isConnected = clientComponent.IsConnected();
    const bool hasEntity = clientComponent.m_entity < INVALID_ENTITY;
    if (!isConnected || !hasEntity) {
        return true;
    }

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    if (gameplayComponent.m_phaseType != PhaseType::PLAY) {
        return true;
    }

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        if (clientComponent.m_isClientPrediction) {
            if (clientComponent.m_isLastInputTransformPending) {
                Input& input = clientComponent.m_inputBuffer.GetLast();
                const InputComponent& inputComponent = input.GetInputComponent();
                F32 dt = input.GetDt();

                std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
                transformComponent.lock()->UpdateTransform(inputComponent.m_acceleration, inputComponent.m_angularAcceleration, dt);
                Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, input.GetFrame());
                transformComponent.lock()->m_inputTransformBuffer.Add(transform);

                clientComponent.m_isLastInputTransformPending = false;
            }
        }
    }

    return true;
}
}
