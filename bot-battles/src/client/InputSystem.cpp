#include "InputSystem.h"

#include "Game.h"
#include "SingletonInputComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
InputSystem::InputSystem()
{
}

//----------------------------------------------------------------------------------------------------
InputSystem::~InputSystem()
{
}

//----------------------------------------------------------------------------------------------------
bool InputSystem::Update()
{
    std::shared_ptr<SingletonInputComponent> input = g_game->GetSingletonInputComponent();

    UpdateSampleInput(*input);

    return true;
}

//----------------------------------------------------------------------------------------------------
void InputSystem::UpdateSampleInput(SingletonInputComponent& input) const
{
    float time = Time::GetInstance().GetTime();
    float nextInputTime = input.GetNextInputTime();
    if (time >= nextInputTime) {
        SampleInput(input);
    }
}

//----------------------------------------------------------------------------------------------------
void InputSystem::SampleInput(SingletonInputComponent& /*input*/) const
{
    // TODO
}
}
