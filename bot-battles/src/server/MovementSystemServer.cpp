#include "MovementSystemServer.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "Input.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "PhysicsComponent.h"
#include "RendererComponent.h"
#include "RigidbodyComponent.h"
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

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    PhysicsComponent& physicsComponent = g_gameServer->GetPhysicsComponent();

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    for (U32 i = 0; i < serverComponent.m_maxInputsPerPaquet; ++i) {
        bool hasInput = false;

        for (auto& entity : m_entities) {
            PlayerID playerID = serverComponent.GetPlayerID(entity);
            if (playerID >= INVALID_PLAYER_ID) {
                continue;
            }

            std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
            std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(entity);

            glm::vec2 linearVelocity = glm::vec2(0.0f, 0.0f);
            F32 angularVelocity = 0.0f;

            std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
            U32 inputCount = clientProxy.lock()->m_inputBuffer.Count();
            if (i < inputCount) {
                hasInput = true;

                U32 inputIndex = clientProxy.lock()->m_inputBuffer.m_front + i;
                const Input& input = clientProxy.lock()->m_inputBuffer.Get(inputIndex);
                const InputComponent& inputComponent = input.GetInputComponent();
                U32 dirtyState = input.GetDirtyState();

                const bool hasLinearVelocity = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
                if (hasLinearVelocity) {
                    // TODO: cap m_acceleration to m_maxAcceleration
                    linearVelocity += inputComponent.m_linearVelocity;
                }
                const bool hasAngularVelocity = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
                if (hasAngularVelocity) {
                    // TODO: cap m_angularAcceleration to m_maxAngularAcceleration
                    angularVelocity += inputComponent.m_angularVelocity;
                }

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

        physicsComponent.Step();

        for (auto& entity : m_entities) {
            PlayerID playerID = serverComponent.GetPlayerID(entity);
            if (playerID >= INVALID_PLAYER_ID) {
                continue;
            }

            std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
            std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(entity);

            std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
            U32 inputCount = clientProxy.lock()->m_inputBuffer.Count();
            if (i < inputCount) {
                U32 inputIndex = clientProxy.lock()->m_inputBuffer.m_front + i;
                const Input& input = clientProxy.lock()->m_inputBuffer.Get(inputIndex);
                U32 dirtyState = input.GetDirtyState();

                const bool hasLinearVelocity = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
                const bool hasAngularVelocity = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
                if (hasLinearVelocity || hasAngularVelocity) {
                    b2Vec2 physicsPosition = rigidbodyComponent.lock()->m_body->GetPosition();
                    transformComponent.lock()->m_position = glm::vec2(METERS_TO_PIXELS(physicsPosition.x), METERS_TO_PIXELS(physicsPosition.y));
                    float32 physicsRotation = rigidbodyComponent.lock()->m_body->GetAngle();
                    transformComponent.lock()->m_rotation = glm::degrees(physicsRotation);
                    ILOG("Server position at frame %u is %f %f rotation %f", input.GetFrame(), transformComponent.lock()->m_position.x, transformComponent.lock()->m_position.y, transformComponent.lock()->m_rotation);

                    Event newEvent;
                    newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
                    newEvent.component.entity = entity;
                    newEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::TRANSFORM_ALL);
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
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(entity);

        if (serverComponent.m_isServerRewind) {
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

    RendererComponent& rendererComponent = g_gameServer->GetRendererComponent();
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    for (const auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);

        glm::vec4 color = Green;
        color.a = 0.5f;
        DebugDraw(rendererComponent, transformComponent, color);
    }

    return true;
}
}
