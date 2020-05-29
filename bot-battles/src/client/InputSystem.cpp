#include "InputSystem.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool InputSystem::Update()
{
    OPTICK_EVENT();

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    const bool hasPlayer = clientComponent.m_playerID < INVALID_PLAYER_ID;
    const bool hasEntity = clientComponent.m_entity < INVALID_ENTITY;
    if (!hasPlayer || !hasEntity) {
        return true;
    }

    /*
    InputComponent& inputComponent = g_gameClient->GetInputComponent();
    inputComponent.m_linearVelocity = glm::vec2(0.0f, 0.0f);
    inputComponent.m_angularVelocity = 0.0f;
    EventComponent& eventComponent = g_game->GetEventComponent();
    U64 dirtyState = 0;
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_A) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_linearVelocity.x = -1.0f;
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);

        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_D) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_linearVelocity.x = 1.0f;
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);

        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_W) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_linearVelocity.y = -1.0f;
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);

        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_S) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_linearVelocity.y = 1.0f;
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);

        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_LEFT) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_angularVelocity = -1.0f;
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_RIGHT) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_angularVelocity = 1.0f;
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_SPACE) == EventComponent::KeyState::DOWN) {
        dirtyState |= static_cast<U64>(InputComponentMemberType::INPUT_SHOOT);
        clientComponent.m_isLastInputWeaponPending = true;
    }

    glm::normalize(inputComponent.m_linearVelocity);
    inputComponent.m_linearVelocity *= inputComponent.m_maxLinearVelocity;
    inputComponent.m_angularVelocity *= inputComponent.m_maxAngularVelocity;
    */

    InputComponent& inputComponent = g_gameClient->GetInputComponent();
    if (inputComponent.m_dirtyState != 0) {
        if (!clientComponent.m_inputBuffer.IsFull()) {
            clientComponent.m_isLastMoveInputPending = (inputComponent.m_dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_LINEAR_VELOCITY)) || (inputComponent.m_dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY));
            clientComponent.m_isLastShootInputPending = (inputComponent.m_dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_PRIMARY_WEAPON)) || (inputComponent.m_dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_SHOOT_SECONDARY_WEAPON));

            Input input = Input(inputComponent, inputComponent.m_dirtyState, clientComponent.m_inputBuffer.m_back, clientComponent.m_interpolationFromFrame, clientComponent.m_interpolationToFrame, clientComponent.m_interpolationPercentage);
            clientComponent.m_inputBuffer.Add(input);
        }
    }

    return true;
}
}
