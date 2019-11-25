#ifndef __TEXTURE_IMPORTER_H__
#define __TEXTURE_IMPORTER_H__

namespace sand
{
	//----------------------------------------------------------------------------------------------------
	class TextureImporter
	{
	public:
		TextureImporter();
		~TextureImporter();

		bool StartUp() const;
		bool ShutDown() const;

		SDL_Texture* LoadFromSprite(const char* path, int& width, int& height) const;
		SDL_Texture* LoadFromText(TTF_Font* font, const char* text, const SDL_Color& color, int& width, int& height) const;
		void UnLoad(SDL_Texture*& texture) const;

	private:
		SDL_Texture* LoadFromSurface(SDL_Surface* surface) const;
	};
}

#endif
