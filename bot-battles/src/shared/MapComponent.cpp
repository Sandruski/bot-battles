#include "MapComponent.h"

#include "Game.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MapComponent::MapComponent()
    : m_tiles()
    , m_tileCount(0, 0)
    , m_tileSize(0, 0)
    , m_scale(1.0f)
{
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
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    realWorld += static_cast<glm::vec2>(windowComponent.m_baseResolution) / 2.0f;

    return realWorld;
}

//----------------------------------------------------------------------------------------------------
glm::uvec2 MapComponent::RealWorldToMap(F32 x, F32 y) const
{
    glm::vec2 world = glm::vec2(x, y);
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    world -= static_cast<glm::vec2>(windowComponent.m_baseResolution) / 2.0f;
    world /= m_scale;
    world /= 0.5f;
    world += static_cast<glm::vec2>(m_tileSize * m_tileCount);
    world /= 2.0f;
    world -= static_cast<glm::vec2>(m_tileSize) / 2.0f;

    glm::uvec2 map = WorldToMap(world.x, world.y);

    return map;
}
}
