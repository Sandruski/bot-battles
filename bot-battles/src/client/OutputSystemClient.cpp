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
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::LOCAL_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool OutputSystemClient::Update()
{
    OPTICK_EVENT();

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (clientComponent.m_isServerReconciliation) {
        for (auto& entity : m_entities) {
            if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
                continue;
            }

            std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
            U32 index = transformComponent.lock()->m_inputTransformBuffer.m_front;
            bool isFound = false;
            while (index < transformComponent.lock()->m_inputTransformBuffer.m_back) {
                const Transform& transform = transformComponent.lock()->m_inputTransformBuffer.Get(index);
                if (transform.GetFrame() == clientComponent.m_lastAckdFrame) {
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

    U32 index = clientComponent.m_inputBuffer.m_front;
    bool isFound = false;
    while (index < clientComponent.m_inputBuffer.m_back) {
        const Input& input = clientComponent.m_inputBuffer.Get(index);
        if (input.GetFrame() == clientComponent.m_lastAckdFrame) {
            isFound = true;
            break;
        }
        ++index;
    }
    if (isFound) {
        clientComponent.m_inputBuffer.Remove(index);
    }

    InputComponent& inputComponent = g_gameClient->GetInputComponent();
    inputComponent.Reset();

    return true;
}
}
