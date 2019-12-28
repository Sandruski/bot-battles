#ifndef __SPRITE_COMPONENT_H__
#define __SPRITE_COMPONENT_H__

#include "Component.h"
#include "SpriteResource.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct SpriteComponent : public Component {

    static ComponentType GetType() { return ComponentType::SPRITE; }
    static std::shared_ptr<SpriteComponent> Instantiate() { return std::make_shared<SpriteComponent>(); }

    SpriteComponent();

    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
    void Read(InputMemoryStream& inputStream, U32 dirtyState) override;

    std::shared_ptr<SpriteResource> m_sprite;
};
}

#endif
