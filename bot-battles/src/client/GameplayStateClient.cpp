#include "GameplayStateClient.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GameplayStateClient::GameplayStateClient()
{
}

//----------------------------------------------------------------------------------------------------
const char* GameplayStateClient::GetName()
{
    return "Gameplay";
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::Enter()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
bool GameplayStateClient::Exit()
{
    return true;
}
}
