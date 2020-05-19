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
F32 TransformComponent::GetPositionX() const
{
    return m_position.x;
}

//----------------------------------------------------------------------------------------------------
F32 TransformComponent::GetPositionY() const
{
    return m_position.y;
}

//----------------------------------------------------------------------------------------------------
F32 TransformComponent::GetRotation() const
{
    return m_rotation;
}

//----------------------------------------------------------------------------------------------------
F32 TransformComponent::GetDirectionX() const
{
    return GetDirection().x;
}

//----------------------------------------------------------------------------------------------------
F32 TransformComponent::GetDirectionY() const
{
    return GetDirection().y;
}
}
