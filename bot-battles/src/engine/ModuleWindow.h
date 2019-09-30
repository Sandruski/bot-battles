#ifndef __MODULE_WINDOW_H__
#define __MODULE_WINDOW_H__

#include "IModule.h"

struct SDL_Window;
struct SDL_Surface;

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class ModuleWindow : public IModule
	{
	public:
		ModuleWindow();
		~ModuleWindow();

		const char* GetName() const override;

		bool StartUp() override;
		bool ShutDown() override;

		SDL_Window* GetWindow() const;

	private:
		SDL_Window* m_window;
		SDL_Surface* m_surface;

		bool m_isInitOk;
	};
}

#endif