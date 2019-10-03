#include "Engine.h"

#include "ModuleFSM.h"
#include "GameplayState.h"

#include "Log.h"
#include "Memory.h"

#include <SDL.h>
#include <cassert>
#include <memory>

enum MainState 
{
    CREATE,
    INIT,
    UPDATE,
    END,
    EXIT,

    STATE_COUNT,
    STATE_INVALID
};

namespace sand
{
	Engine* g_engine;

	//----------------------------------------------------------------------------------------------------
	void StatesSetup()
	{
		U64 id = g_engine->GetFSM().AddState(std::make_shared<GameplayState>());
		g_engine->GetFSM().ChangeState(id);
	}
}

//----------------------------------------------------------------------------------------------------
int main(int /*argc*/, char* /*args*/[])
{
    int ret = EXIT_FAILURE;

    MainState mainState = MainState::CREATE;

	while (mainState != MainState::EXIT)
	{
		switch (mainState)
		{
		case MainState::CREATE:
		{
			LOG("MainState::CREATE");

			sand::EngineConfiguration engineConfiguration(
				"Sand",
				sand::StatesSetup
			);
			sand::g_engine = new sand::Engine(engineConfiguration);
			if (sand::g_engine != nullptr)
			{
				mainState = MainState::INIT;
			}
			else
			{
				LOG("Error: could not create the engine");
				mainState = MainState::EXIT;
			}
			break;
		}

		case MainState::INIT:
		{
			LOG("MainState::INIT");

			bool isInitOk = sand::g_engine->Init();
			if (isInitOk)
			{
				LOG("MainState::UPDATE");
				mainState = MainState::UPDATE;
			}
			else
			{
				LOG("Error: could not initialize the engine");
				mainState = MainState::EXIT;
			}
			break;
		}

		case MainState::UPDATE:
		{
			bool isUpdateOk = sand::g_engine->Update();
			if (!isUpdateOk)
			{
				mainState = MainState::END;
			}
			break;
		}

		case MainState::END:
		{
			LOG("MainState::END");

			bool isEndOk = sand::g_engine->End();
			if (isEndOk)
			{
				ret = EXIT_SUCCESS;
			}
			else
			{
				LOG("Error: could not end the engine");
			}

			mainState = MainState::EXIT;

			break;
		}

		case MainState::EXIT:
		{
			LOG("MainState::EXIT");

			SAFE_DELETE_POINTER(sand::g_engine);

			break;
		}

		default:
		{
			assert(false);
			break;
		}
		}
	}

	SDL_Quit();

    return ret;
}
