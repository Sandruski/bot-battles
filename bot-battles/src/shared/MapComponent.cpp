#include "MapComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MapComponent::MapComponent()
    : m_tilesets()
    , m_tileLayers()
    , m_size(0, 0)
    , m_tileSize(0, 0)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void MapComponent::Read(InputMemoryStream& /*inputStream*/, U32 /*dirtyState*/, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 MapComponent::Write(OutputMemoryStream& /*outputStream*/, U32 /*dirtyState*/) const
{
    U32 writtenState = 0;
    return writtenState;
}
#endif

//----------------------------------------------------------------------------------------------------
glm::uvec2 MapComponent::MapToWorld(U32 i, U32 j)
{
    glm::uvec2 world;
    world.x = i * m_tileSize.x;
    world.y = j * m_tileSize.y;
    return world;
}
}
