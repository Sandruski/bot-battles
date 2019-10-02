#ifndef __MODULE_RENDERER_H__
#define __MODULE_RENDERER_H__

#include "Module.h"

#include <SDL_pixels.h>

struct SDL_Renderer;

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class ModuleRenderer : public Module
	{
	public:
		ModuleRenderer();
		~ModuleRenderer() override;

		bool StartUp() override;
		bool ShutDown() override;

		const char* GetName() const override;

		bool Draw() const;

		SDL_Renderer* GetRenderer() const;

	private:
		void BeginDraw() const;
		void EndDraw() const;

	private:
		SDL_Renderer* m_renderer;
		SDL_Color m_backgroundColor;

		bool m_isInitOk;
		bool m_isDebugDraw;
	};
}

#endif
