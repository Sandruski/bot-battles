#include "InputBuffer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
InputBuffer::InputBuffer()
    : m_front(1)
    , m_back(1)
    , m_inputs()
{
}

//----------------------------------------------------------------------------------------------------
void InputBuffer::Add(const InputComponent& inputComponent, U32 dirtyState)
{
    assert(!IsFull());

    U32 frame = m_back;
    U32 index = GetIndex(frame);
    m_inputs[index] = Input(inputComponent, dirtyState, 1.0f, frame); // TODO: DT SHOULD BE DT AND NOT 1!!!
    ++m_back;
}

//----------------------------------------------------------------------------------------------------
void InputBuffer::Add(const Input& input)
{
    assert(!IsFull());

    U32 frame = input.GetFrame();
    assert(frame == m_back);

    U32 index = GetIndex(frame);
    m_inputs[index] = input;
    ++m_back;
}

//----------------------------------------------------------------------------------------------------
void InputBuffer::Remove(U32 frame)
{
    assert(frame < m_back);
    m_front = frame + 1;
}

//----------------------------------------------------------------------------------------------------
void InputBuffer::Clear()
{
    m_front = m_back;
}

//----------------------------------------------------------------------------------------------------
U32 InputBuffer::GetIndex(U32 frame) const
{
    return frame % m_inputs.size();
}

//----------------------------------------------------------------------------------------------------
const Input& InputBuffer::GetInput(U32 index) const
{
    return m_inputs.at(index);
}

//----------------------------------------------------------------------------------------------------
const Input& InputBuffer::GetLastInput() const
{
    assert(m_back > m_front);
    U32 frame = m_back - 1;
    U32 index = GetIndex(frame);
    return GetInput(index);
}

//----------------------------------------------------------------------------------------------------
bool InputBuffer::IsEmpty() const
{
    return GetCount() == 0;
}

//----------------------------------------------------------------------------------------------------
bool InputBuffer::IsFull() const
{
    return GetCount() == m_inputs.size();
}

//----------------------------------------------------------------------------------------------------
U32 InputBuffer::GetCount() const
{
    return m_back - m_front;
}
}
