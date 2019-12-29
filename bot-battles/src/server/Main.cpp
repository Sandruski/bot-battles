#include "GameServer.h"

#include "FSM.h"
#include "GameplayState.h"

enum MainState {
    CREATE,
    INIT,
    UPDATE,
    END,
    EXIT,

    STATE_COUNT,
    STATE_INVALID
};

namespace sand {
Game* g_game;
GameServer* g_gameServer;

//----------------------------------------------------------------------------------------------------
void StatesSetup()
{
    U32 id = g_game->GetFSM().AddState<GameplayState>();
    g_game->GetFSM().ChangeState(id);
}
}

//----------------------------------------------------------------------------------------------------
int main(int /*argc*/, char* /*args*/[])
{
    int ret = EXIT_FAILURE;

    MainState mainState = MainState::CREATE;

    while (mainState != MainState::EXIT) {
        switch (mainState) {
        case MainState::CREATE: {
            ILOG("MainState::CREATE");

            std::string configPath(sand::CONFIG_DIR);
            configPath.append("server.config");
            sand::g_game = sand::g_gameServer = new sand::GameServer(configPath);
            if (sand::g_game != nullptr) {
                mainState = MainState::INIT;
            } else {
                ELOG("Error: could not create the game");
                mainState = MainState::EXIT;
            }
            break;
        }

        case MainState::INIT: {
            ILOG("MainState::INIT");

            bool isInitOk = sand::g_gameServer->Init();
            if (isInitOk) {
                ILOG("MainState::UPDATE");
                mainState = MainState::UPDATE;
            } else {
                ELOG("Error: could not initialize the game");
                mainState = MainState::EXIT;
            }
            break;
        }

        case MainState::UPDATE: {
            bool isUpdateOk = sand::g_gameServer->DoFrame();
            if (!isUpdateOk) {
                mainState = MainState::END;
            }
            break;
        }

        case MainState::END: {
            ILOG("MainState::END");

            bool isEndOk = sand::g_gameServer->End();
            if (isEndOk) {
                ret = EXIT_SUCCESS;
            } else {
                ELOG("Error: could not end the game");
            }

            mainState = MainState::EXIT;

            break;
        }

        case MainState::EXIT: {
            ILOG("MainState::EXIT");

            SAFE_DELETE_POINTER(sand::g_gameServer);
            sand::g_game = nullptr;

            break;
        }

        default: {
            assert(false);
            break;
        }
        }
    }

    SDL_Quit();

    return ret;
}
