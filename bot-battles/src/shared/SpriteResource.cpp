#include "SpriteResource.h"

#include "Game.h"
#include "TextureImporter.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
SpriteResource::SpriteResource(U32 id, const char* dir, const char* file)
    : Resource(id, dir, file)
    , m_texture(nullptr)
    , m_size()
{
}

//----------------------------------------------------------------------------------------------------
bool SpriteResource::Load()
{
    assert(m_texture == nullptr);

    m_texture = g_game->GetTextureImporter().LoadFromSprite(GetPath(), m_size.x, m_size.y);

    return m_texture != nullptr;
}

//----------------------------------------------------------------------------------------------------
bool SpriteResource::UnLoad()
{
    assert(m_texture != nullptr);

    g_game->GetTextureImporter().UnLoad(m_texture);

    return true;
}

//----------------------------------------------------------------------------------------------------
SDL_Texture* SpriteResource::GetTexture() const
{
    return m_texture;
}

//----------------------------------------------------------------------------------------------------
U32 SpriteResource::GetWidth() const
{
    return m_size.x;
}

//----------------------------------------------------------------------------------------------------
U32 SpriteResource::GetHeight() const
{
    return m_size.y;
}
}
