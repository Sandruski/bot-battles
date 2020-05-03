#include "MovementSystemClient.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "PhysicsComponent.h"
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
bool MovementSystemClient::Update()
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

    PhysicsComponent& physicsComponent = g_gameClient->GetPhysicsComponent();

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameClient->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !rigidbodyComponent.lock()->m_isEnabled) {
            continue;
        }

        if (clientComponent.m_isLastInputTransformPending || clientComponent.m_isLastInputWeaponPending) {
            const Input& input = clientComponent.m_inputBuffer.GetLast();

            if (clientComponent.m_isLastInputTransformPending) {
                glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
                F32 angularVelocity = 0.0f;

                const InputComponent& inputComponent = input.GetInputComponent();
                U32 dirtyState = input.GetDirtyState();

                const bool hasLinearVelocity = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
                if (hasLinearVelocity) {
                    // TODO: cap m_acceleration to m_maxAcceleration
                    velocity += inputComponent.m_linearVelocity;
                }
                const bool hasAngularVelocity = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
                if (hasAngularVelocity) {
                    // TODO: cap m_angularAcceleration to m_maxAngularAcceleration
                    angularVelocity += inputComponent.m_angularVelocity;
                }

                rigidbodyComponent.lock()->m_body->SetLinearVelocity(b2Vec2(PIXELS_TO_METERS(velocity.x), PIXELS_TO_METERS(velocity.y)));
                rigidbodyComponent.lock()->m_body->SetAngularVelocity(glm::radians(angularVelocity));

                physicsComponent.Step();

                b2Vec2 physicsPosition = rigidbodyComponent.lock()->m_body->GetPosition();
                transformComponent.lock()->m_position = glm::vec2(METERS_TO_PIXELS(physicsPosition.x), METERS_TO_PIXELS(physicsPosition.y));
                float32 physicsRotation = rigidbodyComponent.lock()->m_body->GetAngle();
                transformComponent.lock()->m_rotation = glm::degrees(physicsRotation);
                ILOG("Client position at frame %u is %f %f with velocity %f %f", input.GetFrame(), transformComponent.lock()->m_position.x, transformComponent.lock()->m_position.y, inputComponent.m_linearVelocity.x, inputComponent.m_linearVelocity.y);

                clientComponent.m_isLastInputTransformPending = false;
            }

            Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, input.GetFrame());
            transformComponent.lock()->m_inputTransformBuffer.Add(transform);
        }
    }

    return true;
}
}
