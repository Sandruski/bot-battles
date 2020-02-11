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

            if (hasPosition || hasRotation) {
                Event newEvent;
                newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
                newEvent.component.entity = entity;
                newEvent.component.dirtyState = dirtyState;
                NotifyEvent(newEvent);
            }
        }

        if (hasChanged) {
            Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation);
            transformComponent.lock()->m_transformBuffer.Add(transform); // TODO: also remove this transform buffer at some point
            ILOG("Server has changed. Now pos x is: %f", transformComponent.lock()->m_position.x);
            ILOG("SERVER INSERT NUMBER %u", transformComponent.lock()->m_transformBuffer.m_back - 1);
        }
    }

    return true;
}
}
