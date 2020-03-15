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

    U32 GetTexture() const;
    Vec2I GetSize() const;

private:
    U32 m_texture;
    Vec2I m_size;
};
}

#endif
