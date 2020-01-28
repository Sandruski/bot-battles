#include "TransformBuffer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
TransformBuffer::TransformBuffer()
    : m_transforms()
{
}

//----------------------------------------------------------------------------------------------------
void TransformBuffer::Add(const TransformComponent& transformComponent)
{
    m_transforms.emplace_back(std::make_pair(Time::GetInstance().GetFrame(), transformComponent));
}

//----------------------------------------------------------------------------------------------------
void TransformBuffer::RemoveUntilFrame(U32 frame)
{
    while (!m_transforms.empty() && m_transforms.front().first <= frame) {
        m_transforms.pop_front();
    }
}

//----------------------------------------------------------------------------------------------------
void TransformBuffer::Clear()
{
    m_transforms.clear();
}

//----------------------------------------------------------------------------------------------------
TransformComponent& TransformBuffer::GetTransformComponentAtFrame(U32 frame)
{
    for (auto& pair : m_transforms) {
        if (pair.first == frame) {
            return pair.second;
        }
    }

    assert(false);
    return m_transforms.back().second;
}

//----------------------------------------------------------------------------------------------------
U32 TransformBuffer::GetCount() const
{
    return m_transforms.size();
}

//----------------------------------------------------------------------------------------------------
TransformComponent& TransformBuffer::Get(U32 index)
{
    return m_transforms.at(index).second;
}
}
