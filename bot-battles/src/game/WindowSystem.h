#ifndef __WINDOW_SYSTEM_H__
#define __WINDOW_SYSTEM_H__

#include "System.h"
#include "Memory.h"

struct SDL_Window;
struct SDL_Surface;

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class WindowSystem : public System
	{
	public:
		static SystemType GetType()
		{
			return SystemType::INPUT;
		}

	public:
		WindowSystem();
		~WindowSystem() override;

		bool StartUp() override;
		bool ShutDown() override;
	};
}

#endif
