#include "SpriteResource.h"

#include "Game.h"
#include "TextureImporter.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
SpriteResource::SpriteResource(U32 id, const char* dir, const char* file)
    : Resource(id, dir, file)
    , m_texture(0)
    , m_size(0, 0)
{
}

//----------------------------------------------------------------------------------------------------
bool SpriteResource::Load()
{
    assert(m_texture == 0);

    m_texture = g_game->GetTextureImporter().Load(GetPath(), m_size);

    return (m_texture > 0);
}

//----------------------------------------------------------------------------------------------------
bool SpriteResource::UnLoad()
{
    assert(m_texture > 0);

    g_game->GetTextureImporter().UnLoad(m_texture);

    return (m_texture == 0);
}

//----------------------------------------------------------------------------------------------------
U32 SpriteResource::GetTexture() const
{
    return m_texture;
}

//----------------------------------------------------------------------------------------------------
glm::uvec2 SpriteResource::GetSize() const
{
    return m_size;
}
}
