#include "InputSystem.h"

#include "ComponentManager.h"
#include "Game.h"
#include "InputComponent.h"
#include "SingletonInputComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
InputSystem::InputSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::INPUT);
}

//----------------------------------------------------------------------------------------------------
InputSystem::~InputSystem()
{
}

//----------------------------------------------------------------------------------------------------
bool InputSystem::Update()
{
    std::shared_ptr<SingletonInputComponent> singletonInput = g_game->GetSingletonInputComponent();

    UpdateSampleInput(*singletonInput);

    return true;
}

//----------------------------------------------------------------------------------------------------
void InputSystem::UpdateSampleInput(SingletonInputComponent& singletonInput) const
{
    float time = Time::GetInstance().GetTime();
    float nextInputTime = singletonInput.GetNextInputTime();
    if (time >= nextInputTime) {
        SampleInput(singletonInput, time);
    }
}

//----------------------------------------------------------------------------------------------------
void InputSystem::SampleInput(SingletonInputComponent& singletonInput, F32 timestamp) const
{
    for (auto& entity : m_entities) {
        std::shared_ptr<InputComponent> input = g_game->GetComponentManager().GetComponent<InputComponent>(entity);
        singletonInput.AddMove(*input, timestamp);
    }
}
}
