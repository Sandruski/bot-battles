#include "MovementSystemClient.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "RigidbodyComponent.h"
#include "State.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MovementSystemClient::MovementSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::RIGIDBODY);
    m_signature |= 1 << static_cast<U16>(ComponentType::LOCAL_PLAYER); // this should be input component
}

//----------------------------------------------------------------------------------------------------
bool MovementSystemClient::PreUpdate()
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (!clientComponent.m_isClientPrediction) {
        return true;
    }

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameClient->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !rigidbodyComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::vec2 position = transformComponent.lock()->m_position;
        F32 rotation = transformComponent.lock()->m_rotation;
        rigidbodyComponent.lock()->m_body->SetTransform(b2Vec2(PIXELS_TO_METERS(position.x), PIXELS_TO_METERS(position.y)), glm::radians(rotation));
        b2Vec2 newPos = rigidbodyComponent.lock()->m_body->GetPosition();
        position = glm::vec2(newPos.x, newPos.y);

        glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
        F32 angularVelocity = 0.0f;

        if (clientComponent.m_isLastInputTransformPending) {
            const Input& input = clientComponent.m_inputBuffer.GetLast();
            const InputComponent& inputComponent = input.GetInputComponent();
            U32 dirtyState = input.GetDirtyState();

            const bool hasAcceleration = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ACCELERATION);
            if (hasAcceleration) {
                velocity += inputComponent.m_acceleration;
            }
            const bool hasAngularAcceleration = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_ACCELERATION);
            if (hasAngularAcceleration) {
                angularVelocity += inputComponent.m_angularAcceleration;
            }
        }

        // TODO: cap velocity and angular velocity to max before setting them

        rigidbodyComponent.lock()->m_body->SetLinearVelocity(b2Vec2(PIXELS_TO_METERS(velocity.x), PIXELS_TO_METERS(velocity.y)));
        rigidbodyComponent.lock()->m_body->SetAngularVelocity(glm::radians(angularVelocity));
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool MovementSystemClient::PostUpdate()
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (!clientComponent.m_isClientPrediction) {
        return true;
    }

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameClient->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !rigidbodyComponent.lock()->m_isEnabled) {
            continue;
        }

        b2Vec2 physicsPosition = rigidbodyComponent.lock()->m_body->GetPosition();
        transformComponent.lock()->m_position = glm::vec2(METERS_TO_PIXELS(physicsPosition.x), METERS_TO_PIXELS(physicsPosition.y));

        if (clientComponent.m_isLastInputTransformPending || clientComponent.m_isLastInputWeaponPending) {
            const Input& input = clientComponent.m_inputBuffer.GetLast();

            Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, input.GetFrame());
            transformComponent.lock()->m_inputTransformBuffer.Add(transform);
        }

        clientComponent.m_isLastInputTransformPending = false;
    }

    return true;
}
}
