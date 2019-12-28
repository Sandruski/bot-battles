#include "InputSystemClient.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "SingletonInputComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
InputSystemClient::InputSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::INPUT);
}

//----------------------------------------------------------------------------------------------------
InputSystemClient::~InputSystemClient()
{
}

//----------------------------------------------------------------------------------------------------
bool InputSystemClient::Update()
{
    for (auto& entity : m_entities) {

        const U8* keyboardState = SDL_GetKeyboardState(nullptr);
        for (U16 i = 0; i < SDL_NUM_SCANCODES; ++i) {
            if (keyboardState[i] == SDL_KEY_PRESSED) {
                if (m_keyboard[i] == KeyState::IDLE) {
                    m_keyboard[i] = KeyState::DOWN;
                } else {
                    m_keyboard[i] = KeyState::REPEAT;
                }
            } else {
                if (m_keyboard[i] == KeyState::DOWN
                    || m_keyboard[i] == KeyState::REPEAT) {
                    m_keyboard[i] = KeyState::UP;
                } else {
                    m_keyboard[i] = KeyState::IDLE;
                }
            }
        }

        std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetComponentManager().GetComponent<InputComponent>(entity);
        inputComponent.lock()->m_acceleration = Vec2::zero;

        const float multiplier = 100.0f;
        if (m_keyboard[SDL_SCANCODE_A] == KeyState::REPEAT) {
            inputComponent.lock()->m_acceleration.x = -1.0f * multiplier;
        }
        if (m_keyboard[SDL_SCANCODE_D] == KeyState::REPEAT) {
            inputComponent.lock()->m_acceleration.x = 1.0f * multiplier;
        }
        if (m_keyboard[SDL_SCANCODE_W] == KeyState::REPEAT) {
            inputComponent.lock()->m_acceleration.y = -1.0f * multiplier;
        }
        if (m_keyboard[SDL_SCANCODE_S] == KeyState::REPEAT) {
            inputComponent.lock()->m_acceleration.y = 1.0f * multiplier;
        }
    }

    std::shared_ptr<SingletonInputComponent> singletonInput = g_gameClient->GetSingletonInputComponent();

    UpdateSampleInput(*singletonInput);

    return true;
}

//----------------------------------------------------------------------------------------------------
void InputSystemClient::UpdateSampleInput(SingletonInputComponent& singletonInput) const
{
    float time = Time::GetInstance().GetTime();
    float nextInputTime = singletonInput.GetNextInputTime();
    if (time >= nextInputTime) {
        SampleInput(singletonInput, time);
    }
}

//----------------------------------------------------------------------------------------------------
void InputSystemClient::SampleInput(SingletonInputComponent& singletonInput, F32 timestamp) const
{
    for (auto& entity : m_entities) {
        std::weak_ptr<InputComponent> input = g_gameClient->GetComponentManager().GetComponent<InputComponent>(entity);
        singletonInput.ClearMoves(); // TODO: remove to send multiple moves
        singletonInput.AddMove(*input.lock(), static_cast<U32>(ComponentMemberType::INPUT_ACCELERATION), timestamp); // TODO: not only acceleration...
    }
}
}
