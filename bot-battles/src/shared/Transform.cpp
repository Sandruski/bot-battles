#include "Transform.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Transform::Transform()
    : m_position(0.0f, 0.0f)
    , m_rotation(0)
    , m_frame(0)
{
}

//----------------------------------------------------------------------------------------------------
Transform::Transform(const glm::vec2& position, F32 rotation, U32 frame)
    : m_position(position)
    , m_rotation(rotation)
    , m_frame(frame)
{
}

//----------------------------------------------------------------------------------------------------
void Transform::UpdateTransform(const glm::vec2& acceleration, F32 angularAcceleration, F32 dt)
{
    UpdatePosition(acceleration, dt);
    UpdateRotation(angularAcceleration, dt);
}

//----------------------------------------------------------------------------------------------------
void Transform::UpdatePosition(const glm::vec2& acceleration, F32 dt)
{
    m_position.x += acceleration.x * dt;
    m_position.y += acceleration.y * dt;
}

//----------------------------------------------------------------------------------------------------
void Transform::UpdateRotation(F32 angularAcceleration, F32 dt)
{
    m_rotation += angularAcceleration * dt;
}

//----------------------------------------------------------------------------------------------------
U32 Transform::GetFrame() const
{
    return m_frame;
}
}
