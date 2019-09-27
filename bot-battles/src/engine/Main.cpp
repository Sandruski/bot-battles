#include <SDL.h>

#include "Engine.h"
#include "Utils.h"

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
			printf_s("MainState::CREATE");

			sand::g_engine = new sand::Engine("Sand");
			if (sand::g_engine != nullptr)
			{
				mainState = MainState::INIT;
			}
			else
			{
				printf_s("Error: could not create the engine");
				mainState = MainState::EXIT;
			}
			break;
		}

		case MainState::INIT:
		{
			printf_s("MainState::INIT");

			bool isInitOk = sand::g_engine->Init();
			if (isInitOk)
			{
				printf_s("MainState::UPDATE");
				mainState = MainState::UPDATE;
			}
			else
			{
				printf_s("Error: could not initialize the engine");
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
			printf_s("MainState::END");

			bool isEndOk = sand::g_engine->End();
			if (isEndOk)
			{
				ret = EXIT_SUCCESS;
			}
			else
			{
				printf_s("Error: could not end the engine");
			}

			mainState = MainState::EXIT;

			break;
		}

		case MainState::EXIT:
		{
			printf_s("MainState::EXIT");

			SAFE_DELETE_POINTER(sand::g_engine);

			break;
		}
		}
	}

    return ret;
}
