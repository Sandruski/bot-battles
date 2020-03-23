#include "GameplayStateClient.h"

#include "Config.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "LinkingContext.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
const char* GameplayStateClient::GetName() const
{
    return "Gameplay";
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::Enter() const
{
    ILOG("Entering GameplayStateClient...");

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    std::string path;
    path.append(MAPS_DIR);
    path.append(clientComponent.m_map);
    g_gameClient->GetMapImporter().Load(path);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::Exit() const
{
    ILOG("Exiting GameplayStateClient...");

    g_gameClient->GetLinkingContext().ClearEntities();
    g_gameClient->GetEntityManager().ClearEntities();

    return true;
}
}
