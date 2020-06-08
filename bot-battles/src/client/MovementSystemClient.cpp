#include "MovementSystemClient.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "GameplayComponent.h"
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
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
    m_signature |= 1 << static_cast<U16>(ComponentType::LOCAL_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool MovementSystemClient::Update()
{
    OPTICK_EVENT();

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (!clientComponent.lock()->m_isClientPrediction) {
        return true;
    }

    std::weak_ptr<PhysicsComponent> physicsComponent = g_gameClient->GetPhysicsComponent();

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameClient->GetComponentManager().GetComponent<RigidbodyComponent>(entity);

        if (clientComponent.lock()->m_isLastMovementInputPending || clientComponent.lock()->m_isLastWeaponInputPending) {
            const Input& input = clientComponent.lock()->m_inputBuffer.GetLast();

            if (clientComponent.lock()->m_isLastMovementInputPending) {
                glm::vec2 linearVelocity = glm::vec2(0.0f, 0.0f);
                F32 angularVelocity = 0.0f;

                const InputComponent& inputComponent = input.GetInputComponent();
                U64 dirtyState = input.GetDirtyState();

                const bool hasLinearVelocity = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
                if (hasLinearVelocity) {
                    linearVelocity += inputComponent.m_linearVelocity;
                }
                const bool hasAngularVelocity = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
                if (hasAngularVelocity) {
                    angularVelocity += inputComponent.m_angularVelocity;
                }

                if (!std::isfinite(linearVelocity.x)) {
                    linearVelocity.x = 0.0f;
                }

                if (!std::isfinite(linearVelocity.y)) {
                    linearVelocity.y = 0.0f;
                }

                if (!std::isfinite(angularVelocity)) {
                    angularVelocity = 0.0f;
                }

                F32 linearVelocityLength = glm::length(linearVelocity);
                F32 linearVelocityAbsLength = glm::abs(linearVelocityLength);
                if (linearVelocityAbsLength > inputComponent.m_maxLinearVelocity) {
                    linearVelocity = glm::normalize(linearVelocity);
                    linearVelocity *= inputComponent.m_maxLinearVelocity;
                }

                glm::clamp(angularVelocity, -inputComponent.m_maxAngularVelocity, inputComponent.m_maxAngularVelocity);

                if (hasLinearVelocity || hasAngularVelocity) {
                    rigidbodyComponent.lock()->m_body->SetLinearVelocity(b2Vec2(PIXELS_TO_METERS(linearVelocity.x), PIXELS_TO_METERS(linearVelocity.y)));
                    rigidbodyComponent.lock()->m_body->SetAngularVelocity(glm::radians(angularVelocity));

                    rigidbodyComponent.lock()->m_body->SetActive(true);

                    physicsComponent.lock()->Step();

                    b2Vec2 physicsPosition = rigidbodyComponent.lock()->m_body->GetPosition();
                    transformComponent.lock()->m_position = glm::vec2(METERS_TO_PIXELS(physicsPosition.x), METERS_TO_PIXELS(physicsPosition.y));
                    float32 physicsRotation = rigidbodyComponent.lock()->m_body->GetAngle();
                    transformComponent.lock()->m_rotation = glm::degrees(physicsRotation);
                    ILOG("Client position at frame %u is %f %f rot %f", input.GetFrame(), transformComponent.lock()->m_position.x, transformComponent.lock()->m_position.y, transformComponent.lock()->m_rotation);

                    rigidbodyComponent.lock()->m_body->SetActive(false);

                    Event newComponentEvent;
                    newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
                    newComponentEvent.component.dirtyState = static_cast<U64>(ComponentMemberType::TRANSFORM_POSITION) | static_cast<U64>(ComponentMemberType::TRANSFORM_ROTATION);
                    newComponentEvent.component.entity = entity;
                    NotifyEvent(newComponentEvent);
                }

                clientComponent.lock()->m_isLastMovementInputPending = false;
            }

            Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, input.GetFrame());
            transformComponent.lock()->m_inputTransformBuffer.Add(transform);
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool MovementSystemClient::DebugRender()
{
    OPTICK_EVENT();

    LinkingContext& linkingContext = g_gameClient->GetLinkingContext();
    for (const auto& entity : m_entities) {
        NetworkID networkID = linkingContext.GetNetworkID(entity);
        if (networkID >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        DebugDraw(transformComponent);
    }

    return true;
}
}
