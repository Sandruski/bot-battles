#include "OutputSystemServer.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "ServerComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
OutputSystemServer::OutputSystemServer()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool OutputSystemServer::Update()
{
    OPTICK_EVENT();

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

        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        if (clientProxy.expired()) {
            continue;
        }

        clientProxy.lock()->m_inputBuffer.Clear();

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        U32 index = transformComponent.lock()->m_inputTransformBuffer.m_front;
        bool isFound = false;
        while (index < transformComponent.lock()->m_inputTransformBuffer.m_back) {
            const Transform& transform = transformComponent.lock()->m_inputTransformBuffer.Get(index);
            if (transform.GetFrame() == clientProxy.lock()->m_lastAckdFrame) {
                isFound = true;
                break;
            }
            ++index;
        }
        if (isFound) {
            transformComponent.lock()->m_inputTransformBuffer.Remove(index);
        }
    }

    return true;
}
}
