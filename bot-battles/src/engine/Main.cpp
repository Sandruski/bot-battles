#include <SDL.h>

#include "Engine.h"

#include "Log.h"
#include "Memory.h"

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
}

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

			sand::g_engine = new sand::Engine("Sand");
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
			if (!isUpdateOk || sand::g_engine->IsExit())
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
		}
	}

    return ret;
}
