#include "Transform.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Transform::Transform()
    : m_position()
    , m_rotation(0)
    , m_timestamp(0.0f)
    , m_frame(0)
{
}

//----------------------------------------------------------------------------------------------------
Transform::Transform(const Vec3& position, F32 rotation)
    : m_timestamp(0.0f)
    , m_frame(0)
{
    this->m_position = position;
    this->m_rotation = rotation;
}

//----------------------------------------------------------------------------------------------------
Transform::Transform(const Vec3& position, F32 rotation, F32 timestamp, U32 frame)
{
    this->m_position = position;
    this->m_rotation = rotation;
    this->m_timestamp = timestamp;
    this->m_frame = frame;
}

//----------------------------------------------------------------------------------------------------
F32 Transform::GetTimestamp() const
{
    return m_timestamp;
}

//----------------------------------------------------------------------------------------------------
U32 Transform::GetFrame() const
{
    return m_frame;
}
}
