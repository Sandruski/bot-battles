#include "NavigationSystemServer.h"

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
NavigationSystemServer::NavigationSystemServer()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
}

//----------------------------------------------------------------------------------------------------
bool NavigationSystemServer::Update()
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();

    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID == INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);

        for (U32 i = clientProxy.lock()->m_inputBuffer.m_front; i < clientProxy.lock()->m_inputBuffer.m_back; ++i) {
            const Input& input = clientProxy.lock()->m_inputBuffer.Get(i);
            const InputComponent& inputComponent = input.GetInputComponent();
            U32 dirtyState = input.GetDirtyState();
            F32 dt = input.GetDt();
            const bool hasPosition = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION);
            if (hasPosition) {
                transformComponent.lock()->UpdatePosition(inputComponent.m_acceleration, dt);
            }
            const bool hasRotation = dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION);
            if (hasRotation) {
                transformComponent.lock()->UpdateRotation(inputComponent.m_angularAcceleration, dt);
            }

            Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation, Time::GetInstance().GetStartFrameTime(), input.GetFrame());
            transformComponent.lock()->m_transformBuffer.Add(transform); // TODO: also remove this transform buffer at some point

            Event newEvent;
            newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
            newEvent.component.entity = entity;
            newEvent.component.dirtyState = dirtyState;
            NotifyEvent(newEvent);
        }
    }

    return true;
}
}
