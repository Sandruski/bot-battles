#include "InputBuffer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
InputBuffer::InputBuffer()
    : m_inputs()
{
}

//----------------------------------------------------------------------------------------------------
const Input& InputBuffer::Add(const InputComponent& inputComponent, U32 dirtyState)
{
    // TODO: frame or internal counter which can also be used to retrieve the last move frame?
    return m_inputs.emplace_back(inputComponent, dirtyState, 1.0f, Time::GetInstance().GetFrame()); // TODO: USE DT! OMG
}

//----------------------------------------------------------------------------------------------------
const Input& InputBuffer::Add(const Input& input)
{
    return m_inputs.emplace_back(input);
}

//----------------------------------------------------------------------------------------------------
void InputBuffer::Remove(U32 frame)
{
    while (!m_inputs.empty() && m_inputs.front().GetFrame() <= frame) {
        m_inputs.pop_front();
    }
}

//----------------------------------------------------------------------------------------------------
void InputBuffer::Clear()
{
    m_inputs.clear();
}

//----------------------------------------------------------------------------------------------------
const Input& InputBuffer::Get(U32 index) const
{
    return m_inputs.at(index);
}

//----------------------------------------------------------------------------------------------------
const Input& InputBuffer::GetLast() const
{
    return m_inputs.back();
}

//----------------------------------------------------------------------------------------------------
bool InputBuffer::HasInputs() const
{
    return !m_inputs.empty();
}

//----------------------------------------------------------------------------------------------------
U32 InputBuffer::GetCount() const
{
    return m_inputs.size();
}
}
