#include "Transform.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Transform::Transform()
    : m_position()
    , m_rotation(0)
    , m_timestamp(0.0f)
{
}

//----------------------------------------------------------------------------------------------------
Transform::Transform(const Vec3& position, F32 rotation)
    : m_position(position)
    , m_rotation(rotation)
    , m_timestamp(0.0f)
{
}

//----------------------------------------------------------------------------------------------------
Transform::Transform(const Vec3& position, F32 rotation, F32 timestamp)
    : m_position(position)
    , m_rotation(rotation)
    , m_timestamp(timestamp)
{
}

//----------------------------------------------------------------------------------------------------
F32 Transform::GetTimestamp() const
{
    return m_timestamp;
}
}
