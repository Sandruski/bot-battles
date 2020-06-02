#ifndef __MAP_COMPONENT_H__
#define __MAP_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct MapComponent {

    struct Tile {

        enum class TileType : U8 {

            NONE,
            FLOOR,
            BOT_SPAWNER,
            WEAPON_SPAWNER,
            HEALTH_SPAWNER
        };

        Tile()
            : m_tileType(TileType::NONE)
        {
        }

        TileType m_tileType;
    };

    MapComponent();

    Tile& GetTile(U32 i, U32 j)
    {
        return m_tiles.at(i + m_tileCount.x * j);
    }

    const Tile& GetTile(U32 i, U32 j) const
    {
        return m_tiles.at(i + m_tileCount.x * j);
    }

    U32 GetTileCount() const
    {
        return m_tileCount.x * m_tileCount.y;
    }

    glm::uvec2 MapToWorld(U32 i, U32 j) const
    {
        glm::vec2 world;

        world.x = static_cast<F32>(i) * static_cast<F32>(m_tileSize.x);
        world.y = static_cast<F32>(j) * static_cast<F32>(m_tileSize.y);

        return world;
    }

    glm::vec2 MapToRealWorld(U32 i, U32 j) const;

    glm::uvec2 WorldToMap(F32 x, F32 y) const
    {
        glm::uvec2 map;

        map.x = static_cast<U32>(x / static_cast<F32>(m_tileSize.x));
        map.y = static_cast<U32>(y / static_cast<F32>(m_tileSize.y));

        return map;
    }

    glm::uvec2 RealWorldToMap(F32 x, F32 y) const;

    std::vector<Tile> m_tiles;

    glm::uvec2 m_tileCount;
    glm::uvec2 m_tileSize;
    F32 m_scale;
};
}

#endif
