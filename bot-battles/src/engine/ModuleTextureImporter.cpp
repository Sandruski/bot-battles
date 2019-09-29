#include "ModuleTextureImporter.h"

#include "Engine.h"
#include "ModuleRenderer.h"

#include "Log.h"

#include <SDL_image.h>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ModuleTextureImporter::ModuleTextureImporter() : m_isInitOk(false)
	{
	}

	//----------------------------------------------------------------------------------------------------
	ModuleTextureImporter::~ModuleTextureImporter()
	{
	}

	//----------------------------------------------------------------------------------------------------
	const char* ModuleTextureImporter::GetName() const
	{
		return "TextureImporter";
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleTextureImporter::StartUp()
	{
		int imgFlags = IMG_INIT_PNG;
		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			LOG("SDL_image could not be initialized! SDL_image Error: %s", IMG_GetError());
			return m_isInitOk;
		}

		m_isInitOk = true;

		return m_isInitOk;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleTextureImporter::ShutDown()
	{
		return m_isInitOk;
	}

	//----------------------------------------------------------------------------------------------------
	SDL_Texture* ModuleTextureImporter::Load(const char* path) const
	{
		SDL_Texture* texture = nullptr;

		SDL_Surface* surface = IMG_Load(path);
		if (surface == nullptr)
		{
			LOG("Surface could not be loaded from file %s! SDL_image Error: %s", path, IMG_GetError());
			return texture;
		}

		texture = SDL_CreateTextureFromSurface(g_engine->GetRenderer().GetRenderer(), surface);
		if (texture == nullptr)
		{
			LOG("Texture could not be created from file %s! SDL Error: %s", path, SDL_GetError());
		}

		SDL_FreeSurface(surface);

		return texture;
	}

	//----------------------------------------------------------------------------------------------------
	void ModuleTextureImporter::UnLoad(SDL_Texture* texture)
	{
		SDL_DestroyTexture(texture);
		texture = nullptr;
	}
}
