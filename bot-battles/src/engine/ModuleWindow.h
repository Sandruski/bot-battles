#ifndef __MODULE_WINDOW_H__
#define __MODULE_WINDOW_H__

#include "Module.h"
#include "Memory.h"

struct SDL_Window;
struct SDL_Surface;

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class ModuleWindow : public Module
	{
	public:
		static const char* GetName();

	public:
		ModuleWindow();
		~ModuleWindow() override;

		bool StartUp() override;
		bool ShutDown() override;

		SDL_Window* GetWindow() const;

		U32 GetWidth() const { return m_width; }
		U32 GetHeight() const { return m_height; }

	private:
		SDL_Window* m_window;

		U32 m_width;
		U32 m_height;

		bool m_isInitOk;
	};
}

#endif
