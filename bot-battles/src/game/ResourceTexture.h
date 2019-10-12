#ifndef __RESOURCE_TEXTURE_H__
#define __RESOURCE_TEXTURE_H__

#include "Resource.h"

struct SDL_Texture;

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class ResourceTexture : public Resource
	{
	public:
		ResourceTexture(U32 id, const char* dir, const char* file);
		~ResourceTexture() override;

		bool Load() override;
		bool UnLoad() override;

		SDL_Texture* GetTexture() const;

	private:
		SDL_Texture* m_texture;
	};
}

#endif
