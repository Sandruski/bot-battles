#include "Transform.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
Transform::Transform()
    : m_transformComponent()
    , m_frame(0)
{
}

//----------------------------------------------------------------------------------------------------
Transform::Transform(const TransformComponent& transformComponent, U32 frame)
    : m_transformComponent(transformComponent)
    , m_frame(frame)
{
}

//----------------------------------------------------------------------------------------------------
const TransformComponent& Transform::GetTransformComponent() const
{
    return m_transformComponent;
}

//----------------------------------------------------------------------------------------------------
U32 Transform::GetFrame() const
{
    return m_frame;
}
}
