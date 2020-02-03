#include "Transform.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Transform::Transform()
    : m_hasPosition(false)
    , m_hasRotation(false)
    , m_position()
    , m_rotation(0)
    , m_frame(0)
{
}

//----------------------------------------------------------------------------------------------------
Transform::Transform(const Vec3& position, F32 rotation)
    : m_hasPosition(true)
    , m_hasRotation(true)
    , m_frame(0)
{
    this->m_position = position;
    this->m_rotation = rotation;
}

//----------------------------------------------------------------------------------------------------
Transform::Transform(bool hasPosition, bool hasRotation, const Vec3& position, F32 rotation, U32 frame)
{
    this->m_hasPosition = hasPosition;
    this->m_hasRotation = hasRotation;
    this->m_position = position;
    this->m_rotation = rotation;
    this->m_frame = frame;
}

//----------------------------------------------------------------------------------------------------
U32 Transform::GetFrame() const
{
    return m_frame;
}
}
