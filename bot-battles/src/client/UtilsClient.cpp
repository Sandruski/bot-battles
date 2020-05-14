#include "UtilsClient.h"

#include "GameClient.h"
#include "GameplayComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void PyLog(const char* log)
{
    //PYLOG(log);

    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    gameplayComponent.AddLog(log);
}
}
