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
bool OutputSystemServer::PostUpdate()
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

        if (serverComponent.m_isServerRewind) {
            if (!clientProxy->m_inputBuffer.IsEmpty()) {
                Entity entity = serverComponent.GetEntity(playerID);
                if (entity >= INVALID_ENTITY) {
                    continue;
                }
                std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
                if (transformComponent.expired()) {
                    continue;
                }

                Input& input = clientProxy->m_inputBuffer.GetLast();
                U32 index = transformComponent.lock()->m_transformBuffer.m_front;
                bool isFound = false;
                while (index < transformComponent.lock()->m_transformBuffer.m_back) {
                    const Transform& transform = transformComponent.lock()->m_transformBuffer.Get(index);
                    if (transform.GetFrame() == input.m_interpolationFromFrame) {
                        isFound = true;
                        break;
                    }
                    ++index;
                }
                if (isFound) {
                    transformComponent.lock()->m_transformBuffer.Remove(index);
                }
            }
        }

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
