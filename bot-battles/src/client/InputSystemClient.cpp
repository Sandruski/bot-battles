#include "InputSystemClient.h"

#include "ComponentManager.h"
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
        std::shared_ptr<InputComponent> input = g_gameClient->GetComponentManager().GetComponent<InputComponent>(entity);
        input->m_acceleration = Vec2(8.0f, 0.0f);
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
        std::shared_ptr<InputComponent> input = g_gameClient->GetComponentManager().GetComponent<InputComponent>(entity);
        singletonInput.AddMove(*input, static_cast<U32>(ComponentMemberType::INPUT_ACCELERATION), timestamp); // TODO: not only acceleration...
    }
}
}
