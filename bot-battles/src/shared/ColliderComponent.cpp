#include "ColliderComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ColliderComponent::ColliderComponent()
    : m_size(0.0f, 0.0f)
    , m_radius(0.0f)
    , m_shapeType(ShapeType::BOX)
    , m_position(0.0f, 0.0f)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void ColliderComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::COLLIDER_ENABLED)) {
        inputStream.Read(m_isEnabled);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::COLLIDER_SIZE)) {
        inputStream.Read(m_size);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 ColliderComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::COLLIDER_ENABLED)) {
        outputStream.Write(m_isEnabled);
        writtenState |= static_cast<U32>(ComponentMemberType::COLLIDER_ENABLED);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::COLLIDER_SIZE)) {
        outputStream.Write(m_size);
        writtenState |= static_cast<U32>(ComponentMemberType::COLLIDER_SIZE);
    }

    return writtenState;
}
#endif
//----------------------------------------------------------------------------------------------------
glm::vec2 ColliderComponent::GetMin() const
{
    glm::vec2 min = glm::vec2(0.0f, 0.0f);

    switch (m_shapeType) {
    case ShapeType::BOX: {
        min = m_position - m_size / 2.0f;
        break;
    }
    case ShapeType::CIRCLE: {
        min = m_position - m_radius / 2.0f;
        break;
    }
    default: {
        break;
    }
    }

    return min;
}

//----------------------------------------------------------------------------------------------------
glm::vec2 ColliderComponent::GetMax() const
{
    glm::vec2 max = glm::vec2(0.0f, 0.0f);

    switch (m_shapeType) {
    case ShapeType::BOX: {
        max = m_position + m_size / 2.0f;
        break;
    }
    case ShapeType::CIRCLE: {
        max = m_position + m_radius / 2.0f;
        break;
    }
    default: {
        break;
    }
    }

    return max;
}

//----------------------------------------------------------------------------------------------------
bool ColliderComponent::IsColliding(const ColliderComponent& colliderComponent) const
{
    bool isColliding = false;

    switch (m_shapeType) {
    case ShapeType::BOX: {
        switch (colliderComponent.m_shapeType) {
        case ShapeType::BOX: {
            isColliding = TestBoxBox(colliderComponent);
            break;
        }
        case ShapeType::CIRCLE: {
            isColliding = TestBoxCircle(colliderComponent);
            break;
        }
        default: {
            break;
        }
        }
        break;
    }
    case ShapeType::CIRCLE: {
        switch (colliderComponent.m_shapeType) {
        case ShapeType::BOX: {
            isColliding = TestCircleBox(colliderComponent);
            break;
        }
        case ShapeType::CIRCLE: {
            isColliding = TestCircleCircle(colliderComponent);
            break;
        }
        default: {
            break;
        }
        }
        break;
    }
    default: {
        break;
    }
    }

    return isColliding;
}

//----------------------------------------------------------------------------------------------------
bool ColliderComponent::TestCircleCircle(const ColliderComponent& colliderComponent) const
{
    glm::vec2 centerVector = m_position - colliderComponent.m_position;
    F32 centerDistance2 = glm::dot(centerVector, centerVector);
    F32 radiusSum = m_radius + colliderComponent.m_radius;
    F32 radiusSum2 = radiusSum * radiusSum;
    return (centerDistance2 <= radiusSum2);
}

//----------------------------------------------------------------------------------------------------
bool ColliderComponent::TestBoxBox(const ColliderComponent& colliderComponent) const
{
    glm::vec2 minA = GetMin();
    glm::vec2 maxA = GetMax();
    glm::vec2 minB = colliderComponent.GetMin();
    glm::vec2 maxB = colliderComponent.GetMax();
    for (U32 i = 0; i < 2; ++i) {
        if (maxA[i] < minB[i] || minA[i] > maxB[i]) {
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------------------------------------------
bool ColliderComponent::TestCircleBox(const ColliderComponent& colliderComponent) const
{
    glm::vec2 min = colliderComponent.GetMin();
    glm::vec2 max = colliderComponent.GetMax();
    F32 centerDistance2 = 0.0f;
    for (U32 i = 0; i < 2; ++i) {
        F32 v = m_position[i];
        if (v < min[i]) {
            centerDistance2 += (min[i] - v) * (min[i] - v);
        }
        if (v > max[i]) {
            centerDistance2 += (v - max[i]) * (v - max[i]);
        }
    }
    F32 radius2 = m_radius * m_radius;
    return (centerDistance2 <= radius2);
}

//----------------------------------------------------------------------------------------------------
bool ColliderComponent::TestBoxCircle(const ColliderComponent& colliderComponent) const
{
    glm::vec2 min = GetMin();
    glm::vec2 max = GetMax();
    F32 centerDistance2 = 0.0f;
    for (U32 i = 0; i < 2; ++i) {
        F32 v = colliderComponent.m_position[i];
        if (v < min[i]) {
            centerDistance2 += (min[i] - v) * (min[i] - v);
        }
        if (v > max[i]) {
            centerDistance2 += (v - max[i]) * (v - max[i]);
        }
    }
    F32 radius2 = colliderComponent.m_radius * colliderComponent.m_radius;
    return (centerDistance2 <= radius2);
}
}
