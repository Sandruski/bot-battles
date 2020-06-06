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

        Tile();

        // Bot
        TileType GetTileType() const
        {
            return m_tileType;
        }

        TileType m_tileType;
    };

    MapComponent();

    Tile& GetTile(U32 i, U32 j);
    //std::vector<Tile&> GetTileNeighbors(U32 i, U32 j);

    bool IsInBounds(U32 i, U32 j);
    bool IsWalkable(U32 i, U32 j);

    glm::vec2 MapToWorld(U32 i, U32 j) const;
    glm::vec2 MapToRealWorld(U32 i, U32 j) const;
    glm::uvec2 WorldToMap(F32 x, F32 y) const;
    glm::uvec2 RealWorldToMap(F32 x, F32 y) const;

    void Reset();

    // Bot
    std::tuple<F32, F32> GetRealWorldPosition(U32 i, U32 j) const
    {
        glm::vec2 realWorld = MapToRealWorld(i, j);
        return std::make_tuple(realWorld.x, realWorld.y);
    }

    U32 GetTileCountX() const
    {
        return m_tileCount.x;
    }

    U32 GetTileCountY() const
    {
        return m_tileCount.y;
    }

    std::vector<Tile> m_walkability;

    glm::uvec2 m_tileCount;
    glm::uvec2 m_tileSize;
    F32 m_scale;
};
}

#endif
