#ifndef __MODULE_TEXTURE_IMPORTER_H__
#define __MODULE_TEXTURE_IMPORTER_H__

#include "Module.h"

struct SDL_Texture;
struct SDL_Surface;

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class ModuleTextureImporter : public Module
	{
	public:
		ModuleTextureImporter();
		~ModuleTextureImporter();

		bool StartUp() override;
		bool ShutDown() override;

		const char* GetName() const override;

		SDL_Texture* Load(const char* path) const;
		void UnLoad(SDL_Texture* texture);

	private:
		bool m_isInitOk;
	};
}

#endif
