#include "ModuleInput.h"

#include "Log.h"
#include "Defines.h"

#include <SDL.h>
#include <cassert>
#include <cstring>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ModuleInput::ModuleInput() : Module(true),
		m_mousePosition(),
		m_isInitOk(false)
	{
		memset(m_keyboard, static_cast<int>(KeyState::KEY_IDLE), sizeof(KeyState) * SDL_NUM_SCANCODES);
		memset(m_mouse, static_cast<int>(KeyState::KEY_IDLE), sizeof(KeyState) * SDL_BUTTON_X2);
	}

	//----------------------------------------------------------------------------------------------------
	ModuleInput::~ModuleInput()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleInput::StartUp()
	{
		if (SDL_Init(SDL_INIT_EVENTS) == SDL_ERROR)
		{
			LOG("SDL events subsystem could not be initialized! SDL Error: %s", SDL_GetError());
			return m_isInitOk;
		}

		m_isInitOk = true;

		return m_isInitOk;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleInput::ShutDown()
	{
		if (!m_isInitOk)
		{
			return false;
		}

		LOG("Quitting SDL events subsystem");
		SDL_QuitSubSystem(SDL_INIT_EVENTS);

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	const char* ModuleInput::GetName() const
	{
		return "Input";
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleInput::Update()
	{
		if (!m_isInitOk)
		{
			return false;
		}

		const U8* keyboardState = SDL_GetKeyboardState(nullptr);
		for (U16 i = 0; i < SDL_NUM_SCANCODES; ++i)
		{
			if (keyboardState[i] == SDL_KEY_PRESSED)
			{
				if (m_keyboard[i] == KeyState::KEY_IDLE)
				{
					m_keyboard[i] = KeyState::KEY_DOWN;
				}
				else
				{
					m_keyboard[i] = KeyState::KEY_REPEAT;
				}
			}
			else
			{
				if (m_keyboard[i] == KeyState::KEY_DOWN
					|| m_keyboard[i] == KeyState::KEY_REPEAT)
				{
					m_keyboard[i] = KeyState::KEY_UP;
				}
				else
				{
					m_keyboard[i] = KeyState::KEY_IDLE;
				}
			}
		}

		U32 mouseState = SDL_GetMouseState(&m_mousePosition[0], &m_mousePosition[1]);
		for (U8 i = 0; i < SDL_BUTTON_X2; ++i)
		{
			if (mouseState & SDL_BUTTON(i + SDL_BUTTON_LEFT))
			{
				if (m_mouse[i] == KeyState::KEY_IDLE)
				{
					m_mouse[i] = KeyState::KEY_DOWN;
				}
				else
				{
					m_mouse[i] = KeyState::KEY_REPEAT;
				}
			}
			else
			{
				if (m_mouse[i] == KeyState::KEY_DOWN
					|| m_mouse[i] == KeyState::KEY_REPEAT)
				{
					m_mouse[i] = KeyState::KEY_UP;
				}
				else
				{
					m_mouse[i] = KeyState::KEY_IDLE;
				}
			}
		}

		static SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
			{
				// TODO SEND EVENT
				return false;
				break;
			}

			case SDL_WINDOWEVENT:
			{
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_SHOWN:
				case SDL_WINDOWEVENT_MAXIMIZED:
				case SDL_WINDOWEVENT_FOCUS_GAINED:
				{
					// TODO: we show and update m_isRunning accordingly
					// TODO: Event System

					break;
				}

				case SDL_WINDOWEVENT_HIDDEN:
				case SDL_WINDOWEVENT_MINIMIZED:
				case SDL_WINDOWEVENT_FOCUS_LOST:
				{
					// TODO: we hide and update m_isRunning accordingly
					// TODO: Event System
					
					break;
				}

				default:
				{
					break;
				}
				}
				break;
			}

			default:
			{
				break;
			}
			}
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	ModuleInput::KeyState ModuleInput::GetKey(U32 key)
	{
		assert(key > SDL_SCANCODE_UNKNOWN && key < SDL_NUM_SCANCODES);
		return m_keyboard[key];
	}

	//----------------------------------------------------------------------------------------------------
	ModuleInput::KeyState ModuleInput::GetMouseButton(U8 button)
	{
		assert(button >= SDL_BUTTON_LEFT && button <= SDL_BUTTON_X2);
		return m_mouse[button - SDL_BUTTON_LEFT];
	}
}