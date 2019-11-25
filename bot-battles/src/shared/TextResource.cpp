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
		m_text(nullptr),
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

		m_texture = g_game->GetTextureImporter().LoadFromText(m_font->GetFont(), m_text.c_str(), m_color, m_size.x, m_size.y);

		return m_texture != nullptr;
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
		if (m_texture != nullptr)
		{
			UnLoad();
		}

		return Load();
	}
}
