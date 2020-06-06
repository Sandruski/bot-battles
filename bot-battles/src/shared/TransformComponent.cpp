#include "TransformComponent.h"

#include "ComponentMemberTypes.h"
#include "InputComponent.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
TransformComponent::TransformComponent()
    : m_position(0.0f, 0.0f)
    , m_layerType(LayerType::NONE)
    , m_rotation(0.0f)
    , m_scale(1.0f)
    , m_transformBuffer()
    , m_inputTransformBuffer()
{
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
glm::vec2 TransformComponent::GetDirection() const
{
    F32 x = std::cos(glm::radians(m_rotation));
    F32 y = std::sin(glm::radians(m_rotation));

    return glm::vec2(x, y);
}

//----------------------------------------------------------------------------------------------------
std::tuple<F32, F32> TransformComponent::GetPyPosition() const
{
    return std::make_tuple(m_position.x, m_position.y);
}

//----------------------------------------------------------------------------------------------------
F32 TransformComponent::GetPyRotation() const
{
    return m_rotation;
}

//----------------------------------------------------------------------------------------------------
std::tuple<F32, F32> TransformComponent::GetPyDirection() const
{
    glm::vec2 direction = GetDirection();
    return std::make_tuple(direction.x, direction.y);
}
}
