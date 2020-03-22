#include "MovementSystemServer.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "Input.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MovementSystemServer::MovementSystemServer()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
}

//----------------------------------------------------------------------------------------------------
bool MovementSystemServer::Update()
{
    GameComponent& gameComponent = g_gameServer->GetGameComponent();
    if (gameComponent.m_phaseType != PhaseType::PLAY) {
        return true;
    }

    ServerComponent serverComponent = g_gameServer->GetServerComponent();
    U32 frame = MyTime::GetInstance().GetFrame();

    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        bool hasChanged = false;
        for (U32 i = clientProxy.lock()->m_inputBuffer.m_front; i < clientProxy.lock()->m_inputBuffer.m_back; ++i) {
            const Input& input = clientProxy.lock()->m_inputBuffer.Get(i);
            const InputComponent& inputComponent = input.GetInputComponent();
            U32 dirtyState = input.GetDirtyState();
            F32 dt = input.GetDt();
            const bool hasPosition = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION);
            if (hasPosition) {
                transformComponent.lock()->UpdatePosition(inputComponent.m_acceleration, dt);
                hasChanged = true;
            }
            const bool hasRotation = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION);
            if (hasRotation) {
                transformComponent.lock()->UpdateRotation(inputComponent.m_angularAcceleration, dt);
                hasChanged = true;
            }

            if (hasChanged) {
                Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, input.GetFrame());
                transformComponent.lock()->m_inputTransformBuffer.Add(transform); // TODO: also remove this transform buffer at some point

                Event newEvent;
                newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
                newEvent.component.entity = entity;
                newEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::TRANSFORM_ALL);
                NotifyEvent(newEvent);
            }
        }

        Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, frame);
        transformComponent.lock()->m_transformBuffer.Add(transform); // TODO: also remove this transform buffer at some point
        ILOG("Server pos for frame %u is %f %f", frame, transformComponent.lock()->m_position.x, transformComponent.lock()->m_position.y);
    }

    return true;
}
}
