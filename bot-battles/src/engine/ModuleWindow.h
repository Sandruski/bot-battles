#ifndef __MODULE_WINDOW_H__
#define __MODULE_WINDOW_H__

#include "IModule.h"

struct SDL_Window;

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class ModuleWindow : public IModule
	{
	public:
		ModuleWindow();
		~ModuleWindow();

		bool StartUp() override;
		bool ShutDown() override;

		ModuleType GetType() const override;
		bool IsActive() const override;

	private:
		bool CleanUp();

	private:
		SDL_Window *m_window;
		bool m_isActive;
		bool m_isInitOk;
	};
}

#endif
