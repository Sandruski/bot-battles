#include "GameClient.h"

#include "ClientSystem.h"
#include "ConfigClient.h"
#include "InputSystemClient.h"
#include "SystemManager.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GameClient::GameClient(const std::string& configPath)
    : m_configClient(nullptr)
    , m_replicationManager()
    , m_deliveryManager()
    , m_moveComponent()
    , m_clientComponent()
{
    m_config = m_configClient = std::make_shared<ConfigClient>(configPath);
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
