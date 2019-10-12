#ifndef __TEXTURE_IMPORTER_H__
#define __TEXTURE_IMPORTER_H__

struct SDL_Texture;

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class TextureImporter
	{
	public:
		TextureImporter();
		~TextureImporter();

		bool StartUp();

		SDL_Texture* Load(const char* path) const;
		void UnLoad(SDL_Texture* texture) const;
	};
}

#endif
