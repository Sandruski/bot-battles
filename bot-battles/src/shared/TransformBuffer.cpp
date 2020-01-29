#include "TransformBuffer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
TransformBuffer::TransformBuffer()
    : m_front(1)
    , m_back(1)
    , m_transforms()
{
}

//----------------------------------------------------------------------------------------------------
void TransformBuffer::Add(const TransformComponent& transformComponent)
{
    assert(!IsFull());

    U32 frame = m_back;
    U32 index = GetIndex(frame);
    m_transforms[index] = transformComponent;
    ++m_back;
}

//----------------------------------------------------------------------------------------------------
void TransformBuffer::Remove(U32 frame)
{
    assert(frame < m_back);
    m_front = frame + 1;
}

//----------------------------------------------------------------------------------------------------
void TransformBuffer::Clear()
{
    m_front = m_back;
}

//----------------------------------------------------------------------------------------------------
U32 TransformBuffer::GetIndex(U32 frame) const
{
    return frame % m_transforms.size();
}

//----------------------------------------------------------------------------------------------------
TransformComponent& TransformBuffer::GetTransform(U32 index)
{
    return m_transforms.at(index);
}

//----------------------------------------------------------------------------------------------------
TransformComponent& TransformBuffer::GetLastTransform()
{
    assert(m_back > m_front);
    U32 frame = m_back - 1;
    U32 index = GetIndex(frame);
    return GetTransform(index);
}

//----------------------------------------------------------------------------------------------------
bool TransformBuffer::IsFull() const
{
    return GetCount() == m_transforms.size();
}

//----------------------------------------------------------------------------------------------------
U32 TransformBuffer::GetCount() const
{
    return m_back - m_front;
}
}
