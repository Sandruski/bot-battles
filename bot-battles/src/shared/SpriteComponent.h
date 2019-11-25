#ifndef __SPRITE_COMPONENT_H__
#define __SPRITE_COMPONENT_H__

#include "NetComponent.h"
#include "SpriteResource.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct SpriteComponent : public NetComponent {

    static ComponentType GetType() { return ComponentType::SPRITE; }
    static SpriteComponent* Instantiate() { return new SpriteComponent(); }

	SpriteComponent();
	~SpriteComponent();

	U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
	void Read(InputMemoryStream& inputStream, U32 dirtyState) override;

    std::shared_ptr<SpriteResource> m_sprite;
};
}

#endif
