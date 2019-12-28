#include "TextResource.h"

#include "Game.h"
#include "ResourceManager.h"
#include "TextureImporter.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
TextResource::TextResource(U32 id, const char* dir, const char* file)
    : Resource(id, dir, file)
    , m_texture(nullptr)
    , m_font()
    , m_text()
    , m_color()
    , m_size()
{
}

//----------------------------------------------------------------------------------------------------
TextResource::~TextResource()
{
}

//----------------------------------------------------------------------------------------------------
bool TextResource::Load()
{
    assert(m_texture == nullptr);

    if (!m_font.expired()) {
        m_texture = g_game->GetTextureImporter().LoadFromText(m_font.lock()->GetFont(), m_text.c_str(), m_color, m_size.x, m_size.y);

        return m_texture != nullptr;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool TextResource::UnLoad()
{
    assert(m_texture != nullptr);

    g_game->GetTextureImporter().UnLoad(m_texture);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool TextResource::ReLoad()
{
    if (m_texture != nullptr) {
        UnLoad();
    }

    return Load();
}

//----------------------------------------------------------------------------------------------------
SDL_Texture* TextResource::GetTexture() const
{
    return m_texture;
}

//----------------------------------------------------------------------------------------------------
void TextResource::SetFont(std::weak_ptr<FontResource> font)
{
    m_font = font;
}

//----------------------------------------------------------------------------------------------------
void TextResource::SetText(const char* text)
{
    m_text = text;
}

//----------------------------------------------------------------------------------------------------
const char* TextResource::GetText() const
{
    return m_text.c_str();
}

//----------------------------------------------------------------------------------------------------
void TextResource::SetColor(const SDL_Color& color)
{
    m_color = color;
}

//----------------------------------------------------------------------------------------------------
const SDL_Color& TextResource::GetColor() const
{
    return m_color;
}

//----------------------------------------------------------------------------------------------------
U32 TextResource::GetWidth() const
{
    return m_size.x;
}

//----------------------------------------------------------------------------------------------------
U32 TextResource::GetHeight() const
{
    return m_size.y;
}
}
