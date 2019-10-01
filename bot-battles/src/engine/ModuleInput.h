#ifndef __MODULE_INPUT_H__
#define __MODULE_INPUT_H__

#include "IModule.h"
#include "Memory.h"
#include "Utils.h"

#include <SDL_scancode.h>
#include <SDL_mouse.h>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class ModuleInput : public IModule
	{
	public:
		enum class KeyState : int
		{
			KEY_IDLE,
			KEY_DOWN,
			KEY_REPEAT,
			KEY_UP,

			KEYSTATE_COUNT,
			KEYSTATE_INVALID
		};

	public:
		ModuleInput();
		~ModuleInput();

		const char* GetName() const override;

		bool StartUp() override;
		bool ShutDown() override;

		bool Update();

		KeyState GetKey(U32 key);
		KeyState GetMouseButton(U8 button);

	private:
		KeyState m_keyboard[SDL_NUM_SCANCODES];
		KeyState m_mouse[SDL_BUTTON_X2];
		I32 m_mousePosition[2]; // TODO USE VECTOR 2D CLASS

		bool m_isInitOk;
	};
}

#endif