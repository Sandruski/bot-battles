#include "GameClient.h"

#include "ClientSystem.h"
#include "CollisionSystemClient.h"
#include "ComponentManager.h"
#include "ConfigClient.h"
#include "FSM.h"
#include "GameplayStateClient.h"
#include "InputSystemClient.h"
#include "LocalPlayerComponent.h"
#include "MainMenuStateClient.h"
#include "MovementSystemClient.h"
#include "OutputSystemClient.h"
#include "RemotePlayerComponent.h"
#include "RemotePlayerMovementSystem.h"
#include "ScoreboardStateClient.h"
#include "ScriptingSystem.h"
#include "SightSystemClient.h"
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
    ret = m_systemManager->RegisterSystem<RemotePlayerMovementSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<InputSystemClient>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<MovementSystemClient>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<CollisionSystemClient>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<WeaponSystemClient>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<OutputSystemClient>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<ScriptingSystemClient>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<SightSystemClient>();
    if (!ret) {
        return ret;
    }

    // Components
    ret = m_componentManager->RegisterComponent<LocalPlayerComponent>();
    if (!ret) {
        return ret;
    }
    ret = m_componentManager->RegisterComponent<RemotePlayerComponent>();
    if (!ret) {
        return ret;
    }

    // States
    ret = m_fsm->RegisterState<MainMenuStateClient>();
    if (!ret) {
        return ret;
    }
    ret = m_fsm->RegisterState<GameplayStateClient>();
    if (!ret) {
        return ret;
    }
    ret = m_fsm->RegisterState<ScoreboardStateClient>();
    if (!ret) {
        return ret;
    }

    std::weak_ptr<ClientSystem> clientSystem = m_systemManager->GetSystem<ClientSystem>();
    ret = m_fsm->AddObserver(clientSystem);
    if (!ret) {
        return ret;
    }
    ret = clientSystem.lock()->AddObserver(std::weak_ptr<Observer>(m_fsm));
    if (!ret) {
        return ret;
    }
    std::weak_ptr<ScriptingSystemClient> scriptingSystem = m_systemManager->GetSystem<ScriptingSystemClient>();
    ret = clientSystem.lock()->AddObserver(scriptingSystem);
    if (!ret) {
        return ret;
    }
    ret = m_fsm->AddObserver(scriptingSystem);
    if (!ret) {
        return ret;
    }
    ret = scriptingSystem.lock()->AddObserver(std::weak_ptr<Observer>(m_fsm));
    if (!ret) {
        return ret;
    }
    ret = m_physicsComponent.AddObserver(scriptingSystem);
    if (!ret) {
        return ret;
    }
    std::weak_ptr<SightSystemClient> sightSystem = m_systemManager->GetSystem<SightSystemClient>();
    ret = sightSystem.lock()->AddObserver(scriptingSystem);
    if (!ret) {
        return ret;
    }

    ret = Game::Init();

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool GameClient::DoFrame()
{
    bool ret = false;

    std::weak_ptr<ClientSystem> clientSystem = m_systemManager->GetSystem<ClientSystem>();

    //if (m_mainMenuComponent.m_phase != MainMenuComponent::MainMenuPhase::SETUP) {
    clientSystem.lock()->ReceiveIncomingPackets(m_clientComponent);
    //}

    ret = Game::DoFrame();

    //if (m_mainMenuComponent.m_phase != MainMenuComponent::MainMenuPhase::SETUP) {
    clientSystem.lock()->SendOutgoingPackets(m_clientComponent);
    //}

    return ret;
}
}
