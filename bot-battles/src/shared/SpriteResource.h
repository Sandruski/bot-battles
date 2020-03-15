#ifndef __SPRITE_RESOURCE_H__
#define __SPRITE_RESOURCE_H__

#include "Resource.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class SpriteResource : public Resource {
public:
    SpriteResource(U32 id, const char* dir, const char* file);

    bool Load() override;
    bool UnLoad() override;

    U32 GetTexture() const;
    glm::uvec2 GetSize() const;

private:
    U32 m_texture;
    glm::uvec2 m_size;
};
}

#endif
