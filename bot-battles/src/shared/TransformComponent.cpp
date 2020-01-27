#include "TransformComponent.h"

#include "ComponentMemberTypes.h"
#include "InputComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
TransformComponent::TransformComponent()
    : m_startPosition()
    , m_endPosition()
    , m_position()
    , m_rotation(0.0f)
    , m_velocity()
    , m_angularVelocity(0.0f)
#ifdef _CLIENT
    , m_outOfSyncTimestamp(0.0f)
#endif
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
}
