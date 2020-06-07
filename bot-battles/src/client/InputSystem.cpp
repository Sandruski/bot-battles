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

    /*
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    inputComponent.m_linearVelocity = glm::vec2(0.0f, 0.0f);
    inputComponent.m_angularVelocity = 0.0f;
    std::weak_ptr<EventComponent> eventComponent = g_game->GetEventComponent();
    U64 dirtyState = 0;
    if (EventComponent.m_keyboard.at(SDL_SCANCODE_A) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_linearVelocity.x = -1.0f;
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);

        clientComponent.m_isLastInputTransformPending = true;
    }
    if (EventComponent.m_keyboard.at(SDL_SCANCODE_D) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_linearVelocity.x = 1.0f;
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);

        clientComponent.m_isLastInputTransformPending = true;
    }
    if (EventComponent.m_keyboard.at(SDL_SCANCODE_W) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_linearVelocity.y = -1.0f;
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);

        clientComponent.m_isLastInputTransformPending = true;
    }
    if (EventComponent.m_keyboard.at(SDL_SCANCODE_S) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_linearVelocity.y = 1.0f;
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);

        clientComponent.m_isLastInputTransformPending = true;
    }
    if (EventComponent.m_keyboard.at(SDL_SCANCODE_LEFT) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_angularVelocity = -1.0f;
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
        clientComponent.m_isLastInputTransformPending = true;
    }
    if (EventComponent.m_keyboard.at(SDL_SCANCODE_RIGHT) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_angularVelocity = 1.0f;
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
        clientComponent.m_isLastInputTransformPending = true;
    }
    if (EventComponent.m_keyboard.at(SDL_SCANCODE_SPACE) == EventComponent::KeyState::DOWN) {
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_SHOOT);
        clientComponent.m_isLastInputWeaponPending = true;
    }

    glm::normalize(inputComponent.m_linearVelocity);
    inputComponent.m_linearVelocity *= inputComponent.m_maxLinearVelocity;
    inputComponent.m_angularVelocity *= inputComponent.m_maxAngularVelocity;
    */

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
