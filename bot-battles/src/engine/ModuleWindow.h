#ifndef __MODULE_WINDOW_H__
#define __MODULE_WINDOW_H__

#include "IModule.h"
#include "Memory.h"

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

		U32 GetWidth() const { return m_width; }
		U32 GetHeight() const { return m_height; }

	private:
		SDL_Window* m_window;
		SDL_Surface* m_surface;

		U32 m_width;
		U32 m_height;

		bool m_isInitOk;
	};
}

#endif
