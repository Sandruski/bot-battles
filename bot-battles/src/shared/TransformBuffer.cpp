#include "TransformBuffer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
TransformBuffer::TransformBuffer()
    : m_transforms()
{
}

//----------------------------------------------------------------------------------------------------
const Transform& TransformBuffer::Add(const TransformComponent& transformComponent)
{
    // TODO: frame or internal counter which can also be used to retrieve the last move frame?
    return m_transforms.emplace_back(transformComponent, Time::GetInstance().GetFrame()); // TODO: USE DT! OMG
}

//----------------------------------------------------------------------------------------------------
const Transform& TransformBuffer::Add(const Transform& input)
{
    return m_transforms.emplace_back(input);
}

//----------------------------------------------------------------------------------------------------
void TransformBuffer::Remove(U32 frame)
{
    while (!m_transforms.empty() && m_transforms.front().GetFrame() <= frame) {
        m_transforms.pop_front();
    }
}

//----------------------------------------------------------------------------------------------------
void TransformBuffer::Clear()
{
    m_transforms.clear();
}

//----------------------------------------------------------------------------------------------------
const Transform& TransformBuffer::Get(U32 index) const
{
    return m_transforms.at(index);
}

//----------------------------------------------------------------------------------------------------
const Transform& TransformBuffer::GetLast() const
{
    return m_transforms.back();
}

//----------------------------------------------------------------------------------------------------
bool TransformBuffer::HasTransforms() const
{
    return !m_transforms.empty();
}

//----------------------------------------------------------------------------------------------------
U32 TransformBuffer::GetCount() const
{
    return m_transforms.size();
}
}
