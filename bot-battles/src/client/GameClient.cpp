#include "GameClient.h"

#include "ClientSystem.h"
#include "ConfigClient.h"
#include "FSM.h"
#include "InputSystem.h"
#include "MainMenuState.h"
#include "NavigationSystemClient.h"
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

    // Systems
    ret = m_systemManager->RegisterSystem<InputSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<ClientSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<NavigationSystemClient>();
    if (!ret) {
        return ret;
    }

    // States
    ret = m_fsm->RegisterState<MainMenuState>();
    if (!ret) {
        return ret;
    }

    std::weak_ptr<ClientSystem> clientSystem = m_systemManager->GetSystem<ClientSystem>();
    ret = clientSystem.lock()->AddObserver(std::weak_ptr<Observer>(m_fsm));
    if (!ret) {
        return ret;
    }

    ret = Game::Init();

    return ret;
}
}
