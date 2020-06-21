#include "MapComponent.h"

#include "Game.h"
#include "PhysicsComponent.h"
#include "RigidbodyComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MapComponent::MapComponent()
    : m_tileTypes()
    , m_tileCount(0, 0)
    , m_tileSize(0, 0)
    , m_mapScale(1.0f)
{
}

//----------------------------------------------------------------------------------------------------
MapComponent::TileType& MapComponent::GetTileType(const glm::uvec2& mapPosition)
{
    return m_tileTypes.at(mapPosition.x + m_tileCount.x * mapPosition.y);
}

//----------------------------------------------------------------------------------------------------
MapComponent::TileType MapComponent::GetTileType(const glm::uvec2& mapPosition) const
{
    return m_tileTypes.at(mapPosition.x + m_tileCount.x * mapPosition.y);
}

//----------------------------------------------------------------------------------------------------
glm::uvec2 MapComponent::GetMapSize() const
{
    return m_tileSize * m_tileCount;
}

//----------------------------------------------------------------------------------------------------
bool MapComponent::IsVisible(const glm::vec2& fromWorldPosition, const glm::vec2& toWorldPosition) const
{
    glm::uvec2 mapPosition = RealWorldToMap(fromWorldPosition);
    TileType tileType = GetTileType(mapPosition);
    if (tileType == TileType::NONE || tileType == TileType::WALL) {
        return false;
    }

    glm::vec2 vectorToTarget = toWorldPosition - fromWorldPosition;
    glm::vec2 directionToTarget = vectorToTarget;
    if (glm::length(vectorToTarget) > 0.0f) {
        directionToTarget = glm::normalize(vectorToTarget);
    }
    F32 distance = glm::length(vectorToTarget);
    glm::vec2 distanceToTarget = fromWorldPosition + directionToTarget * distance;

    std::weak_ptr<PhysicsComponent> physicsComponent = g_game->GetPhysicsComponent();
    PhysicsComponent::RaycastHit raycastHit;
    if (physicsComponent.lock()->Raycast(fromWorldPosition, distanceToTarget, INVALID_ENTITY, RigidbodyComponent::BodyType::STATIC, raycastHit)) {
        return raycastHit.m_entity == INVALID_ENTITY;
    } else {
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------------------------------
glm::vec2 MapComponent::MapToWorld(const glm::uvec2& mapPosition) const
{
    glm::vec2 world;

    world.x = static_cast<F32>(mapPosition.x) * static_cast<F32>(m_tileSize.x);
    world.y = static_cast<F32>(mapPosition.y) * static_cast<F32>(m_tileSize.y);

    return world;
}

//----------------------------------------------------------------------------------------------------
glm::vec2 MapComponent::MapToRealWorld(const glm::uvec2& mapPosition) const
{
    glm::vec2 realWorld = MapToWorld(mapPosition);
    realWorld += static_cast<glm::vec2>(m_tileSize) / 2.0f;
    realWorld *= 2.0f;
    realWorld -= static_cast<glm::vec2>(m_tileSize * m_tileCount);
    realWorld *= 0.5f;
    realWorld *= m_mapScale;
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    realWorld += static_cast<glm::vec2>(windowComponent.lock()->m_baseResolution) / 2.0f;
    return realWorld;
}

//----------------------------------------------------------------------------------------------------
glm::uvec2 MapComponent::WorldToMap(const glm::vec2& worldPosition) const
{
    glm::uvec2 map;

    map.x = static_cast<U32>(std::round(worldPosition.x / static_cast<F32>(m_tileSize.x)));
    map.y = static_cast<U32>(std::round(worldPosition.y / static_cast<F32>(m_tileSize.y)));

    return map;
}

//----------------------------------------------------------------------------------------------------
glm::uvec2 MapComponent::RealWorldToMap(const glm::vec2& worldPosition) const
{
    glm::vec2 world = worldPosition;
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    world -= static_cast<glm::vec2>(windowComponent.lock()->m_baseResolution) / 2.0f;
    world /= m_mapScale;
    world /= 0.5f;
    world += static_cast<glm::vec2>(m_tileSize * m_tileCount);
    world /= 2.0f;
    world -= static_cast<glm::vec2>(m_tileSize) / 2.0f;

    glm::uvec2 map = WorldToMap(world);

    return map;
}

//----------------------------------------------------------------------------------------------------
void MapComponent::Reset()
{
    m_tileTypes.clear();
    m_tileCount = glm::uvec2(0, 0);
    m_tileSize = glm::uvec2(0, 0);
    m_mapScale = 0.0f;
}
}
