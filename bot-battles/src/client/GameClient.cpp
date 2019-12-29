#include "GameClient.h"

#include "ClientSystem.h"
#include "InputSystemClient.h"
#include "SystemManager.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GameClient::GameClient(const char* configPath)
    : Game(configPath)
    , m_replicationManager()
    , m_deliveryManager()
    , m_moveComponent()
    , m_clientComponent()
{
}

//----------------------------------------------------------------------------------------------------
bool GameClient::Init()
{
    bool ret = false;

    ret = m_systemManager->RegisterSystem<InputSystemClient>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<ClientSystem>();
    if (!ret) {
        return ret;
    }

    ret = Game::Init();

    return ret;
}
}
