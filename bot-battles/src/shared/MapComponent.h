#ifndef __MAP_COMPONENT_H__
#define __MAP_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct MapComponent {

    enum class TileType : U8 {

        NONE,
        WALL,
        FLOOR,
        BOT_SPAWNER,
        WEAPON_SPAWNER,
        HEALTH_SPAWNER
    };

    MapComponent();

    TileType& GetTileType(const glm::uvec2& mapPosition);
    TileType GetTileType(const glm::uvec2& mapPosition) const;
    glm::uvec2 GetMapSize() const;

    bool IsVisible(const glm::vec2& fromWorldPosition, const glm::vec2& toWorldPosition) const;

    glm::vec2 MapToWorld(const glm::uvec2& mapPosition) const;
    glm::vec2 MapToRealWorld(const glm::uvec2& mapPosition) const;
    glm::uvec2 WorldToMap(const glm::vec2& worldPosition) const;
    glm::uvec2 RealWorldToMap(const glm::vec2& realWorldPosition) const;

    void Reset();

#ifdef _CLIENT
    // Python
    TileType GetPyTileType(const std::tuple<U32, U32>& mapPosition) const
    {
        glm::uvec2 newMapPosition = glm::uvec2(std::get<0>(mapPosition), std::get<1>(mapPosition));
        return GetTileType(newMapPosition);
    }

    bool IsPyVisible(const std::tuple<F32, F32>& fromWorldPosition, const std::tuple<F32, F32>& toWorldPosition) const
    {
        glm::vec2 newFromWorldPosition = glm::vec2(std::get<0>(fromWorldPosition), std::get<1>(fromWorldPosition));
        glm::vec2 newToWorldPosition = glm::vec2(std::get<0>(toWorldPosition), std::get<1>(toWorldPosition));
        return IsVisible(newFromWorldPosition, newToWorldPosition);
    }

    std::tuple<F32, F32> GetPyRealWorldPosition(const std::tuple<U32, U32>& mapPosition) const
    {
        glm::uvec2 newMapPosition = glm::uvec2(std::get<0>(mapPosition), std::get<1>(mapPosition));
        glm::vec2 realWorld = MapToRealWorld(newMapPosition);
        return std::make_tuple(realWorld.x, realWorld.y);
    }

    std::tuple<U32, U32> GetPyMapPosition(const std::tuple<F32, F32>& realWorldPosition) const
    {
        glm::vec2 newRealWorldPosition = glm::vec2(std::get<0>(realWorldPosition), std::get<1>(realWorldPosition));
        glm::uvec2 map = RealWorldToMap(newRealWorldPosition);
        return std::make_tuple(map.x, map.y);
    }

    std::tuple<U32, U32> GetPyTileCount() const
    {
        return std::make_tuple(m_tileCount.x, m_tileCount.y);
    }
#endif

    std::vector<TileType> m_tileTypes;
    glm::uvec2 m_tileCount;
    glm::uvec2 m_tileSize;
    F32 m_mapScale;
};
}

#endif
