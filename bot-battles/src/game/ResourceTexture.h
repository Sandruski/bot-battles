#ifndef __RESOURCE_TEXTURE_H__
#define __RESOURCE_TEXTURE_H__

#include "Resource.h"

#include "Vec2I.h"

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
		U32 GetWidth() const;
		U32 GetHeight() const;

	private:
		SDL_Texture* m_texture;
		Vec2I m_size;
	};
}

#endif
