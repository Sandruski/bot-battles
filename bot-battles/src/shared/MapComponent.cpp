#include "MapComponent.h"

#include "Game.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MapComponent::Tile::Tile()
    : m_tileType(TileType::NONE)
{
}

//----------------------------------------------------------------------------------------------------
MapComponent::MapComponent()
    : m_walkability()
    , m_tileCount(0, 0)
    , m_tileSize(0, 0)
    , m_scale(1.0f)
{
}

//----------------------------------------------------------------------------------------------------
MapComponent::Tile& MapComponent::GetTile(U32 i, U32 j)
{
    return m_walkability.at(i + m_tileCount.x * j);
}

//----------------------------------------------------------------------------------------------------
bool MapComponent::IsInBounds(U32 i, U32 j)
{
    return ((0 <= i && i < m_tileCount.x) && (0 <= j && j < m_tileCount.y));
}

//----------------------------------------------------------------------------------------------------
bool MapComponent::IsWalkable(U32 i, U32 j)
{
    Tile& tile = GetTile(i, j);
    return tile.m_tileType != Tile::TileType::NONE;
}

//----------------------------------------------------------------------------------------------------
glm::vec2 MapComponent::MapToWorld(U32 i, U32 j) const
{
    glm::vec2 world;

    world.x = static_cast<F32>(i) * static_cast<F32>(m_tileSize.x);
    world.y = static_cast<F32>(j) * static_cast<F32>(m_tileSize.y);

    return world;
}

//----------------------------------------------------------------------------------------------------
glm::vec2 MapComponent::MapToRealWorld(U32 i, U32 j) const
{
    glm::uvec2 world = MapToWorld(i, j);

    glm::vec2 realWorld = static_cast<glm::vec2>(world);
    realWorld += static_cast<glm::vec2>(m_tileSize) / 2.0f;
    realWorld *= 2.0f;
    realWorld -= static_cast<glm::vec2>(m_tileSize * m_tileCount);
    realWorld *= 0.5f;
    realWorld *= m_scale;
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    realWorld += static_cast<glm::vec2>(windowComponent.lock()->m_baseResolution) / 2.0f;

    return realWorld;
}

//----------------------------------------------------------------------------------------------------
glm::uvec2 MapComponent::WorldToMap(F32 x, F32 y) const
{
    glm::uvec2 map;

    map.x = static_cast<U32>(x / static_cast<F32>(m_tileSize.x));
    map.y = static_cast<U32>(y / static_cast<F32>(m_tileSize.y));

    return map;
}

//----------------------------------------------------------------------------------------------------
glm::uvec2 MapComponent::RealWorldToMap(F32 x, F32 y) const
{
    glm::vec2 world = glm::vec2(x, y);
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    world -= static_cast<glm::vec2>(windowComponent.lock()->m_baseResolution) / 2.0f;
    world /= m_scale;
    world /= 0.5f;
    world += static_cast<glm::vec2>(m_tileSize * m_tileCount);
    world /= 2.0f;
    world -= static_cast<glm::vec2>(m_tileSize) / 2.0f;

    glm::uvec2 map = WorldToMap(world.x, world.y);

    return map;
}

//----------------------------------------------------------------------------------------------------
void MapComponent::Reset()
{
    m_walkability.clear();
}
}
