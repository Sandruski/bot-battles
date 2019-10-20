#include "ResourceTexture.h"

#include "Game.h"
#include "TextureImporter.h"


namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ResourceTexture::ResourceTexture(U32 id, const char* dir, const char* file) : Resource(id, dir, file),
		m_texture(nullptr),
		m_size()
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

		m_texture = g_game->GetTextureImporter().Load(GetPath(), m_size.x, m_size.y);

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

	//----------------------------------------------------------------------------------------------------
	U32 ResourceTexture::GetWidth() const
	{
		return m_size.x;
	}

	//----------------------------------------------------------------------------------------------------
	U32 ResourceTexture::GetHeight() const
	{
		return m_size.y;
	}
}
