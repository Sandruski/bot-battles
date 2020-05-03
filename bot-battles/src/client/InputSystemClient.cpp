#include "InputSystemClient.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool InputSystemClient::Update()
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
    inputComponent.m_linearVelocity = glm::vec2(0.0f, 0.0f);
    inputComponent.m_angularVelocity = 0.0f;
    inputComponent.m_isShooting = false;
    EventComponent& eventComponent = g_game->GetEventComponent();
    U32 dirtyState = 0;
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_A) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_linearVelocity.x = -1.0f;
        dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_A) == EventComponent::KeyState::UP) {
        inputComponent.m_linearVelocity.x = 0.0f;
        dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_D) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_linearVelocity.x = 1.0f;
        dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_D) == EventComponent::KeyState::UP) {
        inputComponent.m_linearVelocity.x = 0.0f;
        dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_W) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_linearVelocity.y = -1.0f;
        dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_W) == EventComponent::KeyState::UP) {
        inputComponent.m_linearVelocity.y = 0.0f;
        dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_S) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_linearVelocity.y = 1.0f;
        dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_S) == EventComponent::KeyState::UP) {
        inputComponent.m_linearVelocity.y = 0.0f;
        dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_LINEAR_VELOCITY);
        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_LEFT) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_angularVelocity = -1.0f;
        dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_RIGHT) == EventComponent::KeyState::REPEAT) {
        inputComponent.m_angularVelocity = 1.0f;
        dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_ANGULAR_VELOCITY);
        clientComponent.m_isLastInputTransformPending = true;
    }
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_SPACE) == EventComponent::KeyState::DOWN) {
        inputComponent.m_isShooting = true;
        dirtyState |= static_cast<U32>(InputComponentMemberType::INPUT_SHOOTING);
        clientComponent.m_isLastInputWeaponPending = true;
    }

    glm::normalize(inputComponent.m_linearVelocity);
    inputComponent.m_linearVelocity *= inputComponent.m_maxLinearVelocity;
    inputComponent.m_angularVelocity *= inputComponent.m_maxAngularVelocity;

    if (!clientComponent.m_inputBuffer.IsFull()) {
        //InputComponent& inputComponent = g_gameClient->GetInputComponent();

        if (dirtyState != 0) {
            Input input = Input(inputComponent, dirtyState, clientComponent.m_inputBuffer.m_back, clientComponent.m_interpolationFromFrame, clientComponent.m_interpolationToFrame, clientComponent.m_interpolationPercentage);
            clientComponent.m_inputBuffer.Add(input);
        }
    } else {
        clientComponent.m_isLastInputTransformPending = false;
        clientComponent.m_isLastInputWeaponPending = false;
    }

    return true;
}
}
