#ifndef __SPRITE_COMPONENT_H__
#define __SPRITE_COMPONENT_H__

#include "Component.h"

#include <memory>

namespace sand
{

	class ResourceTexture;

	//----------------------------------------------------------------------------------------------------
	struct SpriteComponent : public Component
	{
		static ComponentType GetType() { return ComponentType::SPRITE; }

		SpriteComponent(ComponentID id) : Component(id) {}
		~SpriteComponent() override {}

		std::shared_ptr<ResourceTexture> m_texture;
	};
}

#endif