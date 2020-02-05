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

        std::shared_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        assert(clientProxy != nullptr);
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);

        for (U32 i = clientProxy->m_inputBuffer.m_front; i < clientProxy->m_inputBuffer.m_back; ++i) {
            const Input& input = clientProxy->m_inputBuffer.Get(i);
            const InputComponent& inputComponent = input.GetInputComponent();
            F32 dt = input.GetDt();
            transformComponent.lock()->UpdateTransform(inputComponent.m_acceleration, inputComponent.m_angularAcceleration, dt);

            Transform transform = Transform(transformComponent.lock()->m_position, transformComponent.lock()->m_rotation);
            transformComponent.lock()->m_transformBuffer.Add(transform); // TODO: also remove this transform buffer at some point

            Event newEvent;
            newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
            newEvent.component.entity = entity;
            newEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::TRANSFORM_ALL);
            NotifyEvent(newEvent);
        }

        ILOG("POSITIONNNNN %f", transformComponent.lock()->m_position.x);

        if (g_gameServer->GetLinkingContext().GetNetworkID(entity) != INVALID_NETWORK_ID) {
        }
    }

    return true;
}
}
