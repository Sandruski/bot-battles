#ifndef __MAP_COMPONENT_H__
#define __MAP_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

class SpriteResource;

//----------------------------------------------------------------------------------------------------
// Entity Component
struct MapComponent : public Component
#ifdef _CLIENT
    ,
                      public NetworkableReadObject
#elif defined(_SERVER)
    ,
                      public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::MAP; }

    struct Tileset {

        Tileset()
            : m_spriteResource()
            , m_tileCount(0, 0)
            , m_tileSize(0, 0)
            , m_firstGid(0)
            , m_margin(0)
            , m_spacing(0)
        {
        }

        glm::uvec4 GetTileTextureCoords(U32 tileGid)
        {
            glm::uvec4 tilePosition = glm::uvec4(0, 0, 0, 0);
            U32 tileLid = tileGid - m_firstGid;
            ILOG("Local id %u", tileLid);
            glm::uvec2 map = glm::uvec2(0, 0);
            map.x = tileLid % m_tileCount.x;
            map.y = tileLid / m_tileCount.x;
            ILOG("Tile count is %u %u", m_tileCount.x, m_tileCount.y);
            ILOG("x is %u y is %u", map.x, map.y);
            glm::uvec2 world = MapToWorld(map);
            tilePosition.x = world.x;
            tilePosition.y = world.y;
            tilePosition.z = m_tileSize.x;
            tilePosition.w = m_tileSize.y;
            return tilePosition;
        }

        glm::uvec2 MapToWorld(const glm::uvec2& map)
        {
            glm::uvec2 world = glm::uvec2(0, 0);
            world.x = map.x * (m_tileSize.x + m_spacing) + m_margin;
            world.y = map.y * (m_tileSize.y + m_spacing) + m_margin;
            return world;
        }

        std::weak_ptr<SpriteResource> m_spriteResource;

        glm::uvec2 m_tileCount;
        glm::uvec2 m_tileSize;

        U32 m_firstGid;
        U32 m_margin;
        U32 m_spacing;
    };

    MapComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    glm::uvec2 MapToWorld(U32 i, U32 j);

    std::vector<Tileset> m_tilesets;
    std::vector<std::vector<U32>> m_tileLayers;

    glm::uvec2 m_size;
    glm::uvec2 m_tileSize;
};
}

#endif
