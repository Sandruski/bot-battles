#include "Game.h"

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
	Game* g_game;

	//----------------------------------------------------------------------------------------------------
	void StatesSetup()
	{
		U64 id = g_game->GetFSM().AddState(std::make_shared<GameplayState>());
		g_game->GetFSM().ChangeState(id);
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

			sand::GameConfiguration gameConfiguration(
				"Sand",
				sand::StatesSetup
			);
			sand::g_game = new sand::Game(gameConfiguration);
			if (sand::g_game != nullptr)
			{
				mainState = MainState::INIT;
			}
			else
			{
				LOG("Error: could not create the game");
				mainState = MainState::EXIT;
			}
			break;
		}

		case MainState::INIT:
		{
			LOG("MainState::INIT");

			bool isInitOk = sand::g_game->Init();
			if (isInitOk)
			{
				LOG("MainState::UPDATE");
				mainState = MainState::UPDATE;
			}
			else
			{
				LOG("Error: could not initialize the game");
				mainState = MainState::EXIT;
			}
			break;
		}

		case MainState::UPDATE:
		{
			bool isUpdateOk = sand::g_game->Update();
			if (!isUpdateOk)
			{
				mainState = MainState::END;
			}
			break;
		}

		case MainState::END:
		{
			LOG("MainState::END");

			bool isEndOk = sand::g_game->End();
			if (isEndOk)
			{
				ret = EXIT_SUCCESS;
			}
			else
			{
				LOG("Error: could not end the game");
			}

			mainState = MainState::EXIT;

			break;
		}

		case MainState::EXIT:
		{
			LOG("MainState::EXIT");

			SAFE_DELETE_POINTER(sand::g_game);

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
