#include "TransformComponent.h"

#include "ComponentMemberTypes.h"
#include "InputComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
TransformComponent::TransformComponent()
    : m_position(0.0f, 0.0f)
    , m_layerType(LayerType::NONE)
    , m_rotation(0.0f)
    , m_transformBuffer()
    , m_inputTransformBuffer()
{
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::UpdateTransform(const glm::vec2& acceleration, F32 angularAcceleration, F32 dt)
{
    UpdatePosition(acceleration, dt);
    UpdateRotation(angularAcceleration, dt);
}

//----------------------------------------------------------------------------------------------------
void TransformComponent::UpdatePosition(const glm::vec2& acceleration, F32 dt)
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
glm::vec3 TransformComponent::GetPositionAndLayer() const
{
    return glm::vec3(m_position.x, m_position.y, static_cast<F32>(m_layerType));
}

//----------------------------------------------------------------------------------------------------
glm::vec3 TransformComponent::GetDebugPositionAndLayer() const
{
    return glm::vec3(m_position.x, m_position.y, static_cast<F32>(LayerType::DEBUG));
}

//----------------------------------------------------------------------------------------------------
glm::vec2 TransformComponent::GetRotationVector() const
{
    F32 x = std::cos(glm::radians(m_rotation));
    F32 y = std::sin(glm::radians(m_rotation));

    return glm::vec2(x, y);
}

//----------------------------------------------------------------------------------------------------
glm::vec2 TransformComponent::GetPosition() const
{
    return m_position;
}

//----------------------------------------------------------------------------------------------------
F32 TransformComponent::GetRotation() const
{
    return m_rotation;
}
}
