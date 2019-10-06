#ifndef __COMPONENT_RENDERER_H__
#define __COMPONENT_RENDERER_H__

#include "Component.h"

#include <SDL_pixels.h>

struct SDL_Renderer;

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	struct ComponentRenderer : public Component
	{
		static ComponentType GetType() { return ComponentType::RENDERER; }

		ComponentRenderer(ComponentID id) : Component(id),
			m_renderer(nullptr),
			m_backgroundColor(),
			m_isDebugDraw(false)
		{}
		~ComponentRenderer() override {}

		SDL_Renderer* m_renderer;
		SDL_Color m_backgroundColor;
		bool m_isDebugDraw;
	};
}

#endif