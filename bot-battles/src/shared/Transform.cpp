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
    : m_position(position)
    , m_rotation(rotation)
    , m_timestamp(0.0f)
    , m_frame(0)
{
}

//----------------------------------------------------------------------------------------------------
Transform::Transform(const Vec3& position, F32 rotation, F32 timestamp, U32 frame)
    : m_position(position)
    , m_rotation(rotation)
    , m_timestamp(timestamp)
    , m_frame(frame)
{
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
