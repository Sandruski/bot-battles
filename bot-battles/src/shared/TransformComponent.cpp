#include "TransformComponent.h"

#include "ComponentMemberTypes.h"
#include "InputComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
TransformComponent::TransformComponent()
    : m_position()
    , m_rotation(0.0f)
    , m_transformBuffer()
{
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::UpdateTransform(const Vec2& acceleration, F32 angularAcceleration, F32 dt)
{
    UpdatePosition(acceleration, dt);
    UpdateRotation(angularAcceleration, dt);
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::UpdatePosition(const Vec2& acceleration, F32 dt)
{
    m_position.x += acceleration.x * dt;
    m_position.y += acceleration.y * dt;
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::UpdateRotation(F32 angularAcceleration, F32 dt)
{
    m_rotation += angularAcceleration * dt;
}

//----------------------------------------------------------------------------------------------------
Vec2 TransformComponent::GetPosition() const
{
    return Vec2(m_position.x, m_position.y);
}

//----------------------------------------------------------------------------------------------------
Vec2 TransformComponent::GetRotation() const
{
    F32 x = std::cos(DEGREES_TO_RADIANS(m_rotation));
    F32 y = std::sin(DEGREES_TO_RADIANS(m_rotation));

    return Vec2(x, y);
}
}
