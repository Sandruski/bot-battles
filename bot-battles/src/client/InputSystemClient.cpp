#include "InputSystemClient.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool InputSystemClient::PreUpdate()
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Play") {
        return true;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    const bool hasPlayer = clientComponent.m_playerID < INVALID_PLAYER_ID;
    const bool hasEntity = clientComponent.m_entity < INVALID_ENTITY;
    if (!hasPlayer || !hasEntity) {
        return true;
    }

    InputComponent& inputComponent = g_gameClient->GetInputComponent();
    inputComponent.m_acceleration = glm::vec2(0.0f, 0.0f);
    inputComponent.m_angularAcceleration = 0.0f;
    inputComponent.m_isShooting = false;
    EventComponent& eventComponent = g_game->GetEventComponent();
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_A) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_acceleration.x = -1.0f;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_D) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_acceleration.x = 1.0f;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_W) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_acceleration.y = -1.0f;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_S) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_acceleration.y = 1.0f;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_LEFT) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_angularAcceleration = -1.0f;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_RIGHT) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_angularAcceleration = 1.0f;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_SPACE) == EventComponent::KeyState::DOWN) {
        inputComponent.m_isShooting = true;
    }
    glm::normalize(inputComponent.m_acceleration);
    inputComponent.m_acceleration *= inputComponent.m_maxAcceleration;
    inputComponent.m_angularAcceleration *= inputComponent.m_maxAngularAcceleration;

    if (!clientComponent.m_inputBuffer.IsFull()) {
        //InputComponent& inputComponent = g_gameClient->GetInputComponent();
        U32 dirtyState = 0;

        if (!clientComponent.m_isLastInputTransformPending && !clientComponent.m_isLastInputWeaponPending) {
            if (inputComponent.m_acceleration.x != 0.0f || inputComponent.m_acceleration.y != 0.0f) {
                dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_ACCELERATION);
                clientComponent.m_isLastInputTransformPending = true;
            }

            if (inputComponent.m_angularAcceleration != 0.0f) {
                dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_ACCELERATION);
                clientComponent.m_isLastInputTransformPending = true;
            }

            if (inputComponent.m_isShooting) {
                dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_SHOOTING);
                clientComponent.m_isLastInputWeaponPending = true;
            }
        }

        if (dirtyState != 0) {
            Input input = Input(inputComponent, dirtyState, clientComponent.m_inputBuffer.m_back, clientComponent.m_interpolationFromFrame, clientComponent.m_interpolationToFrame, clientComponent.m_interpolationPercentage);
            clientComponent.m_inputBuffer.Add(input);
        }
    }

    return true;
}
}
