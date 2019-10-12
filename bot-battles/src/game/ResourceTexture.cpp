#include "ResourceTexture.h"

#include "Game.h"
#include "TextureImporter.h"

#include <cassert>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ResourceTexture::ResourceTexture(U32 id, const char* dir, const char* file) : Resource(id, dir, file),
		m_texture(nullptr)
	{
	}

	//----------------------------------------------------------------------------------------------------
	ResourceTexture::~ResourceTexture()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool ResourceTexture::Load()
	{
		assert(m_texture == nullptr);

		m_texture = g_game->GetTextureImporter().Load(GetPath());
		return m_texture != nullptr;
	}

	//----------------------------------------------------------------------------------------------------
	bool ResourceTexture::UnLoad()
	{
		assert(m_texture != nullptr);

		g_game->GetTextureImporter().UnLoad(m_texture);
		return true;
	}

	//----------------------------------------------------------------------------------------------------
	SDL_Texture* ResourceTexture::GetTexture() const
	{
		return m_texture;
	}
}
