#include "OutputSystemServer.h"

#include "ClientProxy.h"
#include "ComponentManager.h"
#include "FSM.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "ServerComponent.h"
#include "State.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool OutputSystemServer::Update()
{
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        PlayerID playerID = pair.first;
        std::shared_ptr<ClientProxy> clientProxy = pair.second;

        clientProxy->m_inputBuffer.Clear();

        Entity entity = serverComponent.GetEntity(playerID);
        if (entity >= INVALID_ENTITY) {
            continue;
        }
        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        if (transformComponent.expired()) {
            continue;
        }

        U32 index = transformComponent.lock()->m_inputTransformBuffer.m_front;
        bool isFound = false;
        while (index < transformComponent.lock()->m_inputTransformBuffer.m_back) {
            const Transform& transform = transformComponent.lock()->m_inputTransformBuffer.Get(index);
            if (transform.GetFrame() == clientProxy->m_lastAckdFrame) {
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
