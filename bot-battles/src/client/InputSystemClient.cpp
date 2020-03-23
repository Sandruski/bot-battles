#include "InputSystemClient.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool InputSystemClient::Update()
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    if (gameplayComponent.m_phase != GameplayComponent::GameplayPhase::PLAY) {
        return true;
    }

    InputComponent& inputComponent = g_gameClient->GetInputComponent();
    inputComponent.m_acceleration = glm::vec2(0.0f, 0.0f);
    inputComponent.m_angularAcceleration = 0.0f;
    inputComponent.m_isShooting = false;

    const F32 multiplier = 5.0f;
    EventComponent& eventComponent = g_game->GetEventComponent();
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_A) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_acceleration.x = -1.0f * multiplier;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_D) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_acceleration.x = 1.0f * multiplier;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_W) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_acceleration.y = -1.0f * multiplier;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_S) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_acceleration.y = 1.0f * multiplier;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_LEFT) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_angularAcceleration = -1.0f * multiplier;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_RIGHT) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_angularAcceleration = 1.0f * multiplier;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_S) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_acceleration.y = 1.0f * multiplier;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_SPACE) == EventComponent::KeyState::DOWN) {
        inputComponent.m_isShooting = true;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (!clientComponent.m_inputBuffer.IsFull()) {
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
            Input input = Input(inputComponent, dirtyState, 1.0f, clientComponent.m_inputBuffer.m_back, clientComponent.m_interpolationFromFrame, clientComponent.m_interpolationToFrame, clientComponent.m_interpolationPercentage);
            ILOG("NEW FRAME %u", clientComponent.m_inputBuffer.m_back);
            clientComponent.m_inputBuffer.Add(input);
        }
    }

    return true;
}
}
