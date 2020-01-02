#include "TransformComponent.h"

#include "ComponentMemberTypes.h"
#include "InputComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
TransformComponent::TransformComponent()
    : m_position()
    , m_rotation(0.0f)
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

    return writtenState;
}
#endif

//----------------------------------------------------------------------------------------------------
void TransformComponent::Move(const InputComponent& inputComponent, F32 dt)
{
    m_position.x += inputComponent.m_acceleration.x * dt;
    m_position.y += inputComponent.m_acceleration.y * dt;
    m_rotation += inputComponent.m_angularAcceleration * dt;
}
}
