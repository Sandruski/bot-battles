#include "SingletonInputComponent.h"


namespace sand
{

	//----------------------------------------------------------------------------------------------------
	SingletonInputComponent::KeyState SingletonInputComponent::GetKey(U32 key)
	{
		assert(key > SDL_SCANCODE_UNKNOWN && key < SDL_NUM_SCANCODES);
		return m_keyboard[key];
	}

	//----------------------------------------------------------------------------------------------------
	SingletonInputComponent::KeyState SingletonInputComponent::GetMouseButton(U8 button)
	{
		assert(button >= SDL_BUTTON_LEFT && button <= SDL_BUTTON_X2);
		return m_mouse[button];
	}
}
