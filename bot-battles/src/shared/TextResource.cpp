#include "TextResource.h"

#include "Game.h"
#include "ResourceManager.h"
#include "TextureImporter.h"

namespace sand
{
	//----------------------------------------------------------------------------------------------------
	TextResource::TextResource(U32 id, const char* dir, const char* file) : Resource(id, dir, file),
		m_texture(nullptr),
		m_font(nullptr),
		m_text(),
		m_color(),
		m_size()	
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

		//m_texture = g_game->GetTextureImporter().LoadFromText(m_font->GetFont(), m_text.c_str(), m_color, m_size.x, m_size.y);

		//return m_texture != nullptr;

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
	void TextResource::SetFont(std::shared_ptr<FontResource> font)
	{
		m_font = font;

		ReLoad();
	}

	//----------------------------------------------------------------------------------------------------
	void TextResource::SetText(const char* text)
	{
		m_text = text;

		ReLoad();
	}

	//----------------------------------------------------------------------------------------------------
	void TextResource::SetColor(const SDL_Color& color)
	{
		m_color = color;

		ReLoad();
	}

	//----------------------------------------------------------------------------------------------------
	bool TextResource::ReLoad()
	{
		if (m_texture != nullptr)
		{
			UnLoad();
		}

		return Load();
	}
}
