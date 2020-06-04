#include "OutputSystemClient.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "FSM.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "LinkingContext.h"
#include "State.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
OutputSystemClient::OutputSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
    m_signature |= 1 << static_cast<U16>(ComponentType::LOCAL_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool OutputSystemClient::Update()
{
    OPTICK_EVENT();

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (clientComponent.lock()->m_isServerReconciliation) {
        for (auto& entity : m_entities) {
            if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
                continue;
            }

            std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
            U32 index = transformComponent.lock()->m_inputTransformBuffer.m_front;
            bool isFound = false;
            while (index < transformComponent.lock()->m_inputTransformBuffer.m_back) {
                const Transform& transform = transformComponent.lock()->m_inputTransformBuffer.Get(index);
                if (transform.GetFrame() == clientComponent.lock()->m_lastAckdFrame) {
                    isFound = true;
                    break;
                }
                ++index;
            }
            if (isFound) {
                transformComponent.lock()->m_inputTransformBuffer.Remove(index);
            }
        }
    }

    U32 index = clientComponent.lock()->m_inputBuffer.m_front;
    bool isFound = false;
    while (index < clientComponent.lock()->m_inputBuffer.m_back) {
        const Input& input = clientComponent.lock()->m_inputBuffer.Get(index);
        if (input.GetFrame() == clientComponent.lock()->m_lastAckdFrame) {
            isFound = true;
            break;
        }
        ++index;
    }
    if (isFound) {
        clientComponent.lock()->m_inputBuffer.Remove(index);
    }

    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    inputComponent.lock()->Reset();

    return true;
}
}
