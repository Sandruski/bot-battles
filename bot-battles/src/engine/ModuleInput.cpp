#include "ModuleInput.h"

#include "Utils.h"

#include <SDL.h>

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	ModuleInput::ModuleInput() :
		m_isInitOk(false)
	{
	}

	//----------------------------------------------------------------------------------------------------
	ModuleInput::~ModuleInput()
	{
	}

	//----------------------------------------------------------------------------------------------------
	const char* ModuleInput::GetName() const
	{
		return "Input";
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleInput::StartUp()
	{
		m_isInitOk = true;

		return m_isInitOk;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleInput::ShutDown()
	{
		return false;
	}

	//----------------------------------------------------------------------------------------------------
	bool ModuleInput::Update()
	{
		if (m_isInitOk)
		{
			static SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
				case SDL_QUIT:
					return false;
					break;
				}
			}
		}

		return m_isInitOk;
	}
}