#ifndef __SPRITE_COMPONENT_H__
#define __SPRITE_COMPONENT_H__

#include "ComponentTypes.h"

namespace sand {

class ResourceTexture;

//----------------------------------------------------------------------------------------------------
struct SpriteComponent {
    static ComponentType GetType() { return ComponentType::SPRITE; }
    static SpriteComponent* Instantiate() { return new SpriteComponent(); }

    SpriteComponent()
        : m_texture(nullptr)
    {
    }
    SpriteComponent(std::shared_ptr<ResourceTexture> texture)
        : m_texture(texture)
    {
    }
    ~SpriteComponent() { }

    std::shared_ptr<ResourceTexture> m_texture;
};
}

#endif
