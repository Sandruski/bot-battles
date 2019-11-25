#include "FontImporter.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	FontImporter::FontImporter()
	{
	}

	//----------------------------------------------------------------------------------------------------
	FontImporter::~FontImporter()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool FontImporter::StartUp() const
	{
		if (TTF_Init() == SDL_ERROR)
		{
			ELOG("SDL_ttf could not be initialized! SDL_ttf Error: %s", TTF_GetError());
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool FontImporter::ShutDown() const
	{
		TTF_Quit();

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	TTF_Font* FontImporter::Load(const char* path, U16 size) const
	{
		assert(path != nullptr);

		TTF_Font* font = TTF_OpenFont(path, size);
		if (font == nullptr)
		{
			ELOG("Font could not be opened from file %s! SDL Error: %s", path, SDL_GetError());
		}

		return font;
	}

	//----------------------------------------------------------------------------------------------------
	void FontImporter::UnLoad(TTF_Font*& font) const
	{
		assert(font != nullptr);

		TTF_CloseFont(font);
		font = nullptr;
	}
}
