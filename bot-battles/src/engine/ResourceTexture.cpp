#include "ResourceTexture.h"

#include "Engine.h"
#include "ModuleTextureImporter.h"

#include <cassert>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ResourceTexture::ResourceTexture(const char* dir, const char* file) : Resource(dir, file), 
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

		m_texture = g_engine->GetTextureImporter().Load(GetPath());
		return m_texture != nullptr;
	}

	//----------------------------------------------------------------------------------------------------
	bool ResourceTexture::UnLoad()
	{
		assert(m_texture != nullptr);

		g_engine->GetTextureImporter().UnLoad(m_texture);
		return true;
	}
}