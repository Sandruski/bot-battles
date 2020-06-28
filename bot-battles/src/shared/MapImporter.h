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

        std::string m_spriteFile;

        glm::uvec2 m_tileCount;
        glm::uvec2 m_tileSize;

        U32 m_firstGid;
        U32 m_margin;
        U32 m_spacing;
    };

    struct Property {

        union Value {
            bool boolValue;
            int intValue;
            float floatValue;
        };

        Property()
            : m_name()
            , m_value()
        {
        }

        std::string m_name;
        Value m_value;
    };

    struct Object {

        Object()
            : m_position(0.0f, 0.0f)
            , m_size(0.0f, 0.0f)
            , m_rotation(0.0f)
            , m_name()
            , m_gid(0)
            , m_properties()
        {
        }

        glm::vec2 m_position;
        glm::vec2 m_size;
        F32 m_rotation;
        std::string m_name;
        std::string m_type;
        U32 m_gid;
        std::vector<Property> m_properties;
    };

    struct Objectlayer {

        Objectlayer()
            : m_objects()
            , m_name()
        {
        }

        std::vector<Object> m_objects;
        std::string m_name;
    };

    struct Tilelayer {

        Tilelayer()
            : m_data()
            , m_name()
        {
        }

        U32 GetTileGid(U32 i, U32 j, U32 tileCountX) const
        {
            return m_data.at(i + tileCountX * j);
        }

        std::vector<U32> m_data;
        std::string m_name;
    };

    struct Tilemap {

        Tilemap()
            : m_tilesets()
            , m_tilelayers()
            , m_objectlayers()
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
        std::vector<Objectlayer> m_objectlayers;

        glm::uvec2 m_tileCount;
        glm::uvec2 m_tileSize;
    };

public:
    bool Load(const std::string& path, Tilemap& tilemap) const;
    void Create(const Tilemap& tilemap) const;

private:
    bool LoadTileset(const rapidjson::Value& value, Tileset& tileset) const;
    bool LoadTilelayer(const rapidjson::Value& value, Tilelayer& tileLayer) const;
    bool LoadObjectLayer(const rapidjson::Value& value, Objectlayer& objectLayer) const;
    bool LoadObject(const rapidjson::Value& value, Object& object) const;
    bool LoadProperty(const rapidjson::Value& value, Property& property) const;
};
}

#endif
