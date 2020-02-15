#include "GameClient.h"

#include "ClientSystem.h"
#include "CollisionSystemClient.h"
#include "ConfigClient.h"
#include "FSM.h"
#include "InputSystemClient.h"
#include "MainMenuState.h"
#include "NavigationSystemClient.h"
#include "SystemManager.h"
#include "WeaponSystemClient.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GameClient::GameClient(const std::string& configPath)
    : m_configClient(nullptr)
    , m_clientComponent()
    , m_inputComponent()
{
    m_config = m_configClient = std::make_shared<ConfigClient>(configPath);
}

//----------------------------------------------------------------------------------------------------
bool GameClient::Init()
{
    bool ret = false;

    // Systems
    ret = m_systemManager->RegisterSystem<ClientSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<InputSystemClient>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<WeaponSystemClient>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<NavigationSystemClient>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<CollisionSystemClient>();
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

//----------------------------------------------------------------------------------------------------
bool GameClient::Update()
{
    std::weak_ptr<ClientSystem> clientSystem = m_systemManager->GetSystem<ClientSystem>();

    clientSystem.lock()->ReceiveIncomingPackets(m_clientComponent);

    Game::Update();

    clientSystem.lock()->SendOutgoingPackets(m_clientComponent);

    return true;
}
}
