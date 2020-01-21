#include "InputSystem.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "MoveComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
InputSystem::InputSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::INPUT);
}

//----------------------------------------------------------------------------------------------------
bool InputSystem::Update()
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

    MoveComponent& moveComponent = g_gameClient->GetMoveComponent();

    UpdateSampleInput(moveComponent);

    return true;
}

//----------------------------------------------------------------------------------------------------
void InputSystem::UpdateSampleInput(MoveComponent& moveComponent) const
{
    F32 time = Time::GetInstance().GetTime();
    F32 nextMoveTime = moveComponent.GetNextMoveTime();
    if (time >= nextMoveTime) {
        SampleInput(moveComponent, Time::GetInstance().GetStartFrameTime());
        moveComponent.m_lastMoveTimestamp = time;
    }
}

//----------------------------------------------------------------------------------------------------
void InputSystem::SampleInput(MoveComponent& moveComponent, F32 timestamp) const
{
    for (auto& entity : m_entities) {
        std::weak_ptr<InputComponent> input = g_gameClient->GetComponentManager().GetComponent<InputComponent>(entity);
        moveComponent.m_moves.AddMove(*input.lock(), static_cast<U32>(ComponentMemberType::INPUT_ACCELERATION), timestamp); // TODO: not only acceleration...
        moveComponent.m_isLastMovePending = true;
    }
}
}
