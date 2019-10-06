#ifndef __COMPONENT_SPRITE_H__
#define __COMPONENT_SPRITE_H__

#include "Component.h"

#include <memory>

namespace sand
{

	class ResourceTexture;

	//----------------------------------------------------------------------------------------------------
	struct ComponentSprite : public Component
	{
		static ComponentType GetType() { return ComponentType::SPRITE; }

		ComponentSprite(ComponentID id) : Component(id) {}
		~ComponentSprite() override {}

		std::shared_ptr<ResourceTexture> m_texture;
	};
}

#endif