#ifndef __MAP_IMPORTER_H__
#define __MAP_IMPORTER_H__

namespace sand {

struct MapComponent;

//----------------------------------------------------------------------------------------------------
class MapImporter {
public:
    struct Tileset {

        Tileset()
            : m_spriteFile()
            , m_tileCount(0, 0)
            , m_tileSize(0, 0)
            , m_firstGid(0)
            , m_margin(0)
            , m_spacing(0)
        {
        }

        glm::uvec4 GetTextureCoords(U32 tileGid) const
        {
            glm::uvec4 tilePosition = glm::uvec4(0, 0, 0, 0);
            tilePosition.z = m_tileSize.x;
            tilePosition.w = m_tileSize.y;

            U32 tileLid = tileGid - m_firstGid;
            glm::uvec2 map = glm::uvec2(0, 0);
            map.x = tileLid % m_tileCount.x;
            map.y = tileLid / m_tileCount.x;

            glm::uvec2 world = MapToWorld(map);
            tilePosition.x = world.x;
            tilePosition.y = world.y;

            return tilePosition;
        }

        glm::uvec2 MapToWorld(const glm::uvec2& map) const
        {
            glm::uvec2 world = glm::uvec2(0, 0);

            world.x = map.x * (m_tileSize.x + m_spacing) + m_margin;
            world.y = map.y * (m_tileSize.y + m_spacing) + m_margin;

            return world;
        }

        //std::weak_ptr<SpriteResource> m_spriteResource;
        std::string m_spriteFile;

        glm::uvec2 m_tileCount;
        glm::uvec2 m_tileSize;

        U32 m_firstGid;
        U32 m_margin;
        U32 m_spacing;
    };

    struct Tilelayer {

        Tilelayer()
            : m_data()
        {
        }

        U32 GetTileGid(U32 i, U32 j, U32 tileCountX) const
        {
            return m_data.at(i + tileCountX * j);
        }

        std::vector<U32> m_data;
    };

    struct Tilemap {

        Tilemap()
            : m_tilesets()
            , m_tilelayers()
            , m_tileCount(0)
            , m_tileSize(0)
        {
        }

        glm::uvec2 MapToWorld(U32 i, U32 j) const
        {
            glm::uvec2 world;

            world.x = i * m_tileSize.x;
            world.y = j * m_tileSize.y;

            return world;
        }

        const Tileset& GetTileset(U32 tileGid) const
        {
            std::vector<Tileset>::const_iterator it = m_tilesets.begin();

            while (it != m_tilesets.end()) {
                if (tileGid < (*it).m_firstGid) {
                    return *std::prev(it, 1);
                }
                ++it;
            }

            return *std::prev(it, 1);
        }

        std::vector<Tileset> m_tilesets;
        std::vector<Tilelayer> m_tilelayers;

        glm::uvec2 m_tileCount;
        glm::uvec2 m_tileSize;
    };

public:
    bool Load(const std::string& path) const;

private:
    std::vector<Tileset> LoadTilesets(const rapidjson::Value& value) const;
    Tilelayer LoadTilelayer(const rapidjson::Value& value) const;
    void LoadObjectLayer(const rapidjson::Value& value) const;
};
}

#endif
