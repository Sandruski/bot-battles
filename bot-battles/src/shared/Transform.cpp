#include "Transform.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Transform::Transform()
    : m_position(0.0f, 0.0f, 0.0f)
    , m_rotation(0)
    , m_frame(0)
{
}

//----------------------------------------------------------------------------------------------------
Transform::Transform(const glm::vec3& position, F32 rotation, U32 frame)
    : m_position(position)
    , m_rotation(rotation)
    , m_frame(frame)
{
}

//----------------------------------------------------------------------------------------------------
U32 Transform::GetFrame() const
{
    return m_frame;
}
}
