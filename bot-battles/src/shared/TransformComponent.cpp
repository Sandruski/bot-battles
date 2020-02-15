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

#ifdef _SERVER
//----------------------------------------------------------------------------------------------------
U32 TransformComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION)) {
        outputStream.Write(m_position);
        writtenState |= static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION)) {
        outputStream.Write(m_rotation);
        writtenState |= static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION);
    }
    /*
    if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_VELOCITY)) {
        outputStream.Write(m_velocity);
        writtenState |= static_cast<U32>(ComponentMemberType::TRANSFORM_VELOCITY);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ANGULAR_VELOCITY)) {
        outputStream.Write(m_angularVelocity);
        writtenState |= static_cast<U32>(ComponentMemberType::TRANSFORM_ANGULAR_VELOCITY);
    }*/

    return writtenState;
}
#endif

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
