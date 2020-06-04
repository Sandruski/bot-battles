#include "UtilsClient.h"

#include "GameClient.h"
#include "GameplayComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void PyLog(const char* log)
{
    //PYLOG(log);

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    gameplayComponent.lock()->AddLog(log);
}
}
