#ifndef __SPRITE_COMPONENT_H__
#define __SPRITE_COMPONENT_H__

namespace sand {

class ResourceTexture;

//----------------------------------------------------------------------------------------------------
struct SpriteComponent {
    static ComponentType GetType() { return ComponentType::SPRITE; }

    SpriteComponent()
        : m_texture(nullptr)
    {
    }
    ~SpriteComponent() { }

    std::shared_ptr<ResourceTexture> m_texture;
};
}

#endif
