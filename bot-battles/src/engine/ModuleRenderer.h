#ifndef __MODULE_RENDERER_H__
#define __MODULE_RENDERER_H__

#include "IModule.h"

struct SDL_Renderer;

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class ModuleRenderer : public IModule
	{
	public:
		ModuleRenderer();
		~ModuleRenderer();

		const char* GetName() const override;

		bool StartUp() override;
		bool ShutDown() override;

		bool Draw();

		SDL_Renderer* GetRenderer() const;

	private:
		SDL_Renderer* m_renderer;

		bool m_isInitOk;
	};
}

#endif
