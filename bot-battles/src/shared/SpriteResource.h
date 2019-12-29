#ifndef __SPRITE_RESOURCE_H__
#define __SPRITE_RESOURCE_H__

#include "Resource.h"

#include "Vec2I.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class SpriteResource : public Resource {
public:
    SpriteResource(U32 id, const char* dir, const char* file);

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
