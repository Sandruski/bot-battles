#include "TextureImporter.h"

#include "Game.h"

#include "SingletonRendererComponent.h"

#include "Log.h"

#include <SDL_image.h>
#include <cassert>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	TextureImporter::TextureImporter()
	{
	}

	//----------------------------------------------------------------------------------------------------
	TextureImporter::~TextureImporter()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool TextureImporter::StartUp()
	{
		int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			LOG("SDL_image could not be initialized! SDL_image Error: %s", IMG_GetError());
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	SDL_Texture* TextureImporter::Load(const char* path) const
	{
		assert(path != nullptr);

		SDL_Texture* texture = nullptr;

		SDL_Surface* surface = IMG_Load(path);
		if (surface == nullptr)
		{
			LOG("Surface could not be loaded from file %s! SDL_image Error: %s", path, IMG_GetError());
			return texture;
		}

		std::shared_ptr<SingletonRendererComponent> renderer = g_game->GetSingletonRendererComponent();

		texture = SDL_CreateTextureFromSurface(renderer->m_renderer, surface);
		if (texture == nullptr)
		{
			LOG("Texture could not be created from file %s! SDL Error: %s", path, SDL_GetError());
		}

		SDL_FreeSurface(surface);

		return texture;
	}

	//----------------------------------------------------------------------------------------------------
	void TextureImporter::UnLoad(SDL_Texture* texture) const
	{
		assert(texture != nullptr);

		SDL_DestroyTexture(texture);
		texture = nullptr;
	}
}
