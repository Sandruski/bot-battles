#include "TextureImporter.h"

#include "Game.h"

#include "SingletonRendererComponent.h"

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
	bool TextureImporter::StartUp() const
	{
		int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			ELOG("SDL_image could not be initialized! SDL_image Error: %s", IMG_GetError());
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool TextureImporter::ShutDown() const
	{
		IMG_Quit();

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	SDL_Texture* TextureImporter::LoadFromSprite(const char* path, int& width, int& height) const
	{
		assert(path != nullptr);

		SDL_Surface* surface = IMG_Load(path);
		if (surface == nullptr)
		{
			ELOG("Surface could not be loaded from sprite! SDL_image Error: %s", IMG_GetError());
			return nullptr;
		}

		SDL_Texture* texture = LoadFromSurface(surface);
		if (texture == nullptr)
		{
			ELOG("Texture could not be created from sprite!");
		}
		else
		{
			width = surface->w;
			height = surface->h;
		}

		SDL_FreeSurface(surface);

		return texture;
	}

	//----------------------------------------------------------------------------------------------------
	SDL_Texture* TextureImporter::LoadFromText(TTF_Font* font, const char* text, const SDL_Color& color, int& width, int& height) const
	{
		assert(font != nullptr && text != nullptr);

		SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
		if (surface == nullptr)
		{
			ELOG("Surface could not be loaded from text! SDL_ttf Error: %s", TTF_GetError());
			return nullptr;
		}

		SDL_Texture* texture = LoadFromSurface(surface);
		if (texture == nullptr)
		{
			ELOG("Texture could not be created from text!");
		}
		else
		{
			width = surface->w;
			height = surface->h;
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

	//----------------------------------------------------------------------------------------------------
	SDL_Texture* TextureImporter::LoadFromSurface(SDL_Surface* surface) const
	{
		assert(surface != nullptr);

		std::shared_ptr<SingletonRendererComponent> renderer = g_game->GetSingletonRendererComponent();
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer->m_renderer, surface);
		if (texture == nullptr)
		{
			ELOG("Texture could not be created from surface! SDL Error: %s", SDL_GetError());
		}

		return texture;
	}
}
