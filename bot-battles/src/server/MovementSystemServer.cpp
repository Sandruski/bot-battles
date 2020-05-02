#include "MovementSystemServer.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "Input.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "PhysicsComponent.h"
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
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();

    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !rigidbodyComponent.lock()->m_isEnabled) {
            continue;
        }

        glm::vec2 velocity = glm::vec2(0.0f, 0.0f);
        F32 angularVelocity = 0.0f;

        glm::vec2 position = transformComponent.lock()->m_position;
        F32 rotation = transformComponent.lock()->m_rotation;

        PhysicsComponent& physicsComponent = g_gameServer->GetPhysicsComponent();
        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        for (U32 i = clientProxy.lock()->m_inputBuffer.m_front; i < clientProxy.lock()->m_inputBuffer.m_back; ++i) {
            const Input& input = clientProxy.lock()->m_inputBuffer.Get(i);
            const InputComponent& inputComponent = input.GetInputComponent();
            U32 dirtyState = input.GetDirtyState();

            // TODO: cap acceleration to max acceleration and angular acceleration to max angular acceleration

            const bool hasAcceleration = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ACCELERATION);
            if (hasAcceleration) {
                velocity += inputComponent.m_acceleration;
                position += inputComponent.m_acceleration * physicsComponent.m_timeStep;
            }
            const bool hasAngularAcceleration = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_ACCELERATION);
            if (hasAngularAcceleration) {
                angularVelocity += inputComponent.m_angularAcceleration;
                rotation += inputComponent.m_angularAcceleration * physicsComponent.m_timeStep;
            }

            Transform transform = Transform(position, rotation, input.GetFrame());
            transformComponent.lock()->m_inputTransformBuffer.Add(transform);
            ILOG("Server position at frame %u: %f %f", transform.GetFrame(), transform.m_position.x, transform.m_position.y);
            ILOG("With acceleration: %f %f", inputComponent.m_acceleration.x, inputComponent.m_acceleration.y);
        }

        rigidbodyComponent.lock()->m_body->SetLinearVelocity(b2Vec2(PIXELS_TO_METERS(velocity.x), PIXELS_TO_METERS(velocity.y)));
        rigidbodyComponent.lock()->m_body->SetAngularVelocity(glm::radians(angularVelocity));
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool MovementSystemServer::PostUpdate()
{
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    U32 frame = MyTime::GetInstance().GetFrame();

    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameServer->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
        if (!transformComponent.lock()->m_isEnabled || !rigidbodyComponent.lock()->m_isEnabled) {
            continue;
        }

        bool hasChanged = false;
        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        for (U32 i = clientProxy.lock()->m_inputBuffer.m_front; i < clientProxy.lock()->m_inputBuffer.m_back; ++i) {
            const Input& input = clientProxy.lock()->m_inputBuffer.Get(i);
            U32 dirtyState = input.GetDirtyState();

            const bool hasAcceleration = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ACCELERATION);
            if (hasAcceleration) {
                hasChanged = true;
                break;
            }
            const bool hasAngularAcceleration = dirtyState & static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_ACCELERATION);
            if (hasAngularAcceleration) {
                hasChanged = true;
                break;
            }
        }

        b2Vec2 physicsPosition = rigidbodyComponent.lock()->m_body->GetPosition();
        transformComponent.lock()->m_position = glm::vec2(METERS_TO_PIXELS(physicsPosition.x), METERS_TO_PIXELS(physicsPosition.y));
        float32 physicsRotation = rigidbodyComponent.lock()->m_body->GetAngle();
        transformComponent.lock()->m_rotation = glm::degrees(physicsRotation);

        if (hasChanged) {
            Event newEvent;
            newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
            newEvent.component.entity = entity;
            newEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::TRANSFORM_ALL);
            NotifyEvent(newEvent);
        }

        if (serverComponent.m_isServerRewind) {
            Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, frame);
            transformComponent.lock()->m_transformBuffer.Add(transform);
        }
    }

    return true;
}
}
