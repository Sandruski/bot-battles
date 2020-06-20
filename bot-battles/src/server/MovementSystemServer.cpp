#include "MovementSystemServer.h"

#include "BotComponent.h"
#include "ClientProxy.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "Input.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "PhysicsComponent.h"
#include "RendererComponent.h"
#include "RigidbodyComponent.h"
#include "ServerComponent.h"
#include "State.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MovementSystemServer::MovementSystemServer()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::RIGIDBODY);
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool MovementSystemServer::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool MovementSystemServer::Update()
{
    OPTICK_EVENT();

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    std::weak_ptr<PhysicsComponent> physicsComponent = g_gameServer->GetPhysicsComponent();

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    for (U32 i = 0; i < serverComponent.lock()->m_maxInputsPerPaquet; ++i) {
        bool hasInput = false;

        for (auto& entity : m_entities) {
            PlayerID playerID = serverComponent.lock()->GetPlayerID(entity);
            if (playerID >= INVALID_PLAYER_ID) {
                continue;
            }

            std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
            std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
            std::weak_ptr<BotComponent> botComponent = g_gameServer->GetComponentManager().GetComponent<BotComponent>(entity);

            if (botComponent.lock()->m_actionType == BotComponent::ActionType::KILL || botComponent.lock()->m_actionType == BotComponent::ActionType::DIE) {
                continue;
            }

            glm::vec2 linearVelocity = glm::vec2(0.0f, 0.0f);
            F32 angularVelocity = 0.0f;

            std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
            U32 inputCount = clientProxy.lock()->m_inputBuffer.Count();
            if (i < inputCount) {
                hasInput = true;

                U32 inputIndex = clientProxy.lock()->m_inputBuffer.m_front + i;
                const Input& input = clientProxy.lock()->m_inputBuffer.Get(inputIndex);
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
                if (linearVelocityLength > rigidbodyComponent.lock()->m_maxLinearVelocity) {
                    linearVelocity = glm::normalize(linearVelocity);
                    linearVelocity *= rigidbodyComponent.lock()->m_maxLinearVelocity;
                }

                glm::clamp(angularVelocity, -rigidbodyComponent.lock()->m_maxAngularVelocity, rigidbodyComponent.lock()->m_maxAngularVelocity);

                if (hasLinearVelocity || hasAngularVelocity) {
                    rigidbodyComponent.lock()->m_body->SetLinearVelocity(b2Vec2(PIXELS_TO_METERS(linearVelocity.x), PIXELS_TO_METERS(linearVelocity.y)));
                    rigidbodyComponent.lock()->m_body->SetAngularVelocity(glm::radians(angularVelocity));

                    rigidbodyComponent.lock()->m_body->SetActive(true);
                }
            }
        }

        if (!hasInput) {
            break;
        }

        physicsComponent.lock()->Step();

        for (auto& entity : m_entities) {
            PlayerID playerID = serverComponent.lock()->GetPlayerID(entity);
            if (playerID >= INVALID_PLAYER_ID) {
                continue;
            }

            std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
            std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(entity);

            std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
            U32 inputCount = clientProxy.lock()->m_inputBuffer.Count();
            if (i < inputCount) {
                U32 inputIndex = clientProxy.lock()->m_inputBuffer.m_front + i;
                const Input& input = clientProxy.lock()->m_inputBuffer.Get(inputIndex);
                U64 dirtyState = input.GetDirtyState();

                const bool hasLinearVelocity = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
                const bool hasAngularVelocity = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
                if (hasLinearVelocity || hasAngularVelocity) {
                    b2Vec2 physicsPosition = rigidbodyComponent.lock()->m_body->GetPosition();
                    transformComponent.lock()->m_position = glm::vec2(METERS_TO_PIXELS(physicsPosition.x), METERS_TO_PIXELS(physicsPosition.y));
                    float32 physicsRotation = rigidbodyComponent.lock()->m_body->GetAngle();
                    transformComponent.lock()->m_rotation = glm::degrees(physicsRotation);

                    Event newEvent;
                    newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
                    newEvent.component.entity = entity;
                    newEvent.component.dirtyState = static_cast<U64>(ComponentMemberType::TRANSFORM_POSITION) | static_cast<U64>(ComponentMemberType::TRANSFORM_ROTATION);
                    NotifyEvent(newEvent);
                }

                Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, input.GetFrame());
                transformComponent.lock()->m_inputTransformBuffer.Add(transform);
            }

            rigidbodyComponent.lock()->m_body->SetActive(false);
        }
    }

    U32 frame = MyTime::GetInstance().GetFrame();

    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.lock()->GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(entity);

        if (serverComponent.lock()->m_isServerRewind) {
            Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, frame);
            transformComponent.lock()->m_transformBuffer.Add(transform);
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool MovementSystemServer::DebugRender()
{
    OPTICK_EVENT();

    std::weak_ptr<RendererComponent> rendererComponent = g_gameServer->GetRendererComponent();
    if (!rendererComponent.lock()->m_isDebugDrawBot) {
        return true;
    }

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    for (const auto& entity : m_entities) {
        PlayerID playerID = serverComponent.lock()->GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        DebugDraw(transformComponent);
    }

    return true;
}
}
