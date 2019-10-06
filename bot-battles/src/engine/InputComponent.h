#ifndef __INPUT_COMPONENT_H__
#define __INPUT_COMPONENT_H__

#include "Component.h"

#include "Vector2Int.h"

#include <cstring>
#include <SDL_scancode.h>
#include <SDL_mouse.h>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	struct InputComponent : public Component
	{
		enum class KeyState
		{
			KEY_IDLE,
			KEY_DOWN,
			KEY_REPEAT,
			KEY_UP,

			KEYSTATE_COUNT,
			KEYSTATE_INVALID
		};

		static ComponentType GetType() { return ComponentType::INPUT; }

		InputComponent(ComponentID id) : Component(id)
		{
			memset(m_keyboard, static_cast<int>(KeyState::KEY_IDLE), sizeof(KeyState) * SDL_NUM_SCANCODES);
			memset(m_mouse, static_cast<int>(KeyState::KEY_IDLE), sizeof(KeyState) * SDL_BUTTON_X2);
		}
		~InputComponent() override {}

		KeyState GetKey(U32 key);
		KeyState GetMouseButton(U8 button);

		KeyState m_keyboard[SDL_NUM_SCANCODES];
		KeyState m_mouse[SDL_BUTTON_X2];
		Vector2Int m_mousePosition;
	};
}

#endif