#include "InputComponent.h"

#include <cassert>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	InputComponent::KeyState InputComponent::GetKey(U32 key)
	{
		assert(key > SDL_SCANCODE_UNKNOWN && key < SDL_NUM_SCANCODES);
		return m_keyboard[key];
	}

	//----------------------------------------------------------------------------------------------------
	InputComponent::KeyState InputComponent::GetMouseButton(U8 button)
	{
		assert(button >= SDL_BUTTON_LEFT && button <= SDL_BUTTON_X2);
		return m_mouse[button];
	}
}