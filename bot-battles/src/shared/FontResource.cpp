#include "FontResource.h"

#include "FontImporter.h"
#include "Game.h"

namespace sand
{
	//----------------------------------------------------------------------------------------------------
	FontResource::FontResource(U32 id, const char* dir, const char* file) : Resource(id, dir, file),
		m_font(nullptr),
		m_size(0)
	{
	}

	//----------------------------------------------------------------------------------------------------
	FontResource::~FontResource()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool FontResource::Load()
	{
		assert(m_font == nullptr);

		m_font = g_game->GetFontImporter().Load(GetPath(), m_size);

		return m_font != nullptr;
	}

	//----------------------------------------------------------------------------------------------------
	bool FontResource::UnLoad()
	{
		assert(m_font != nullptr);

		g_game->GetFontImporter().UnLoad(m_font);

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	void FontResource::SetSize(U16 size)
	{
		m_size = size;

		ReLoad();
	}

	//----------------------------------------------------------------------------------------------------
	bool FontResource::ReLoad()
	{
		if (m_font != nullptr)
		{
			UnLoad();
		}

		return Load();
	}
}
