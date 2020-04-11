#include "RestartStateServer.h"

#include "ConfigServer.h"
#include "FSM.h"
#include "GameServer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string RestartStateServer::GetName() const
{
    return "Restart";
}

//----------------------------------------------------------------------------------------------------
bool RestartStateServer::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RestartStateServer::Update() const
{
    // V
    ChangeToGameplay();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RestartStateServer::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
void RestartStateServer::ChangeToGameplay() const
{
    g_gameServer->GetFSM().ChangeState("Gameplay");
}
}
