#include "InputSystem.h"

#include "Game.h"
#include "ComponentManager.h"
#include "InputComponent.h"

#include "Log.h"
#include "Defines.h"

#include <SDL.h>
#include <cassert>
#include <cstring>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	InputSystem::InputSystem()
	{
	}

	//----------------------------------------------------------------------------------------------------
	InputSystem::~InputSystem()
	{
	}

	//----------------------------------------------------------------------------------------------------
	bool InputSystem::StartUp()
	{
		if (SDL_Init(SDL_INIT_EVENTS) == SDL_ERROR)
		{
			LOG("SDL events subsystem could not be initialized! SDL Error: %s", SDL_GetError());
			return false;
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------------
	bool InputSystem::Update(F32 /*dt*/)
	{
		InputComponent& input = g_game->GetInputComponent();

		const U8* keyboardState = SDL_GetKeyboardState(nullptr);
		for (U16 i = 0; i < SDL_NUM_SCANCODES; ++i)
		{
			if (keyboardState[i] == SDL_KEY_PRESSED)
			{
				if (input.m_keyboard[i] == InputComponent::KeyState::KEY_IDLE)
				{
					input.m_keyboard[i] = InputComponent::KeyState::KEY_DOWN;
				}
				else
				{
					input.m_keyboard[i] = InputComponent::KeyState::KEY_REPEAT;
				}
			}
			else
			{
				if (input.m_keyboard[i] == InputComponent::KeyState::KEY_DOWN
					|| input.m_keyboard[i] == InputComponent::KeyState::KEY_REPEAT)
				{
					input.m_keyboard[i] = InputComponent::KeyState::KEY_UP;
				}
				else
				{
					input.m_keyboard[i] = InputComponent::KeyState::KEY_IDLE;
				}
			}
		}

		U32 mouseState = SDL_GetMouseState(&input.m_mousePosition.x, &input.m_mousePosition.y);
		for (U8 i = 0; i < SDL_BUTTON_X2; ++i)
		{
			if (mouseState & SDL_BUTTON(i + SDL_BUTTON_LEFT))
			{
				if (input.m_mouse[i] == InputComponent::KeyState::KEY_IDLE)
				{
					input.m_mouse[i] = InputComponent::KeyState::KEY_DOWN;
				}
				else
				{
					input.m_mouse[i] = InputComponent::KeyState::KEY_REPEAT;
				}
			}
			else
			{
				if (input.m_mouse[i] == InputComponent::KeyState::KEY_DOWN
					|| input.m_mouse[i] == InputComponent::KeyState::KEY_REPEAT)
				{
					input.m_mouse[i] = InputComponent::KeyState::KEY_UP;
				}
				else
				{
					input.m_mouse[i] = InputComponent::KeyState::KEY_IDLE;
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
	bool InputSystem::ShutDown()
	{
		LOG("Quitting SDL events subsystem");
		SDL_QuitSubSystem(SDL_INIT_EVENTS);

		return true;
	}
}