#include "InputSystem.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "InputComponent.h"
#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool InputSystem::Update()
{
    OPTICK_EVENT();

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    const bool hasPlayer = clientComponent.lock()->m_playerID < INVALID_PLAYER_ID;
    const bool hasEntity = clientComponent.lock()->m_entity < INVALID_ENTITY;
    if (!hasPlayer || !hasEntity) {
        return true;
    }

    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    if (inputComponent.lock()->m_dirtyState != 0) {
        if (!clientComponent.lock()->m_inputBuffer.IsFull()) {
            clientComponent.lock()->m_isLastMovementInputPending = (inputComponent.lock()->m_dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY))
                || (inputComponent.lock()->m_dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY));
            clientComponent.lock()->m_isLastWeaponInputPending = (inputComponent.lock()->m_dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_PRIMARY_WEAPON))
                || (inputComponent.lock()->m_dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_SECONDARY_WEAPON))
                || (inputComponent.lock()->m_dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_RELOAD));
            clientComponent.lock()->m_isLastHealthInputPending = (inputComponent.lock()->m_dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_HEAL));

            Input input = Input(*inputComponent.lock(), inputComponent.lock()->m_dirtyState, clientComponent.lock()->m_inputBuffer.m_back, clientComponent.lock()->m_interpolationFromFrame, clientComponent.lock()->m_interpolationToFrame, clientComponent.lock()->m_interpolationPercentage);
            clientComponent.lock()->m_inputBuffer.Add(input);
        }
    }

    return true;
}
}
