#ifndef __RENDERER_COMPONENT_H__
#define __RENDERER_COMPONENT_H__

#include "Component.h"

#include <SDL_pixels.h>

struct SDL_Renderer;

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	struct RendererComponent : public Component
	{
		static ComponentType GetType() { return ComponentType::RENDERER; }

		RendererComponent(ComponentID id) : Component(id),
			m_renderer(nullptr),
			m_backgroundColor(),
			m_isDebugDraw(false)
		{}
		~RendererComponent() override {}

		SDL_Renderer* m_renderer;
		SDL_Color m_backgroundColor;
		bool m_isDebugDraw;
	};
}

#endif