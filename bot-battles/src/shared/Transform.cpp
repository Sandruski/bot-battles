#include "Transform.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Transform::Transform()
    : m_position()
    , m_rotation(0)
    , m_frame(0)
{
}

//----------------------------------------------------------------------------------------------------
Transform::Transform(const Vec3& position, F32 rotation, U32 frame)
{
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
