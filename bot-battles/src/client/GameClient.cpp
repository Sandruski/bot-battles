#include "GameClient.h"

#include "ClientSystem.h"
#include "CollisionSystemClient.h"
#include "ComponentManager.h"
#include "ConfigClient.h"
#include "FSM.h"
#include "GameplayStateClient.h"
#include "InputSystem.h"
#include "LocalPlayerComponent.h"
#include "MainMenuStateClient.h"
#include "MovementSystemClient.h"
#include "OutputSystemClient.h"
#include "RemotePlayerComponent.h"
#include "RemotePlayerMovementSystem.h"
#include "RendererSystem.h"
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
    ret = m_systemManager->RegisterSystem<InputSystem>();
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
    ret = m_systemManager->RegisterSystem<ScriptingSystem>();
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

    ret = Game::Init();

    std::weak_ptr<ClientSystem> clientSystem = m_systemManager->GetSystem<ClientSystem>();
    ret = m_fsm->AddObserver(clientSystem);
    if (!ret) {
        return ret;
    }
    ret = clientSystem.lock()->AddObserver(std::weak_ptr<Observer>(m_fsm));
    if (!ret) {
        return ret;
    }
    std::weak_ptr<ScriptingSystem> scriptingSystem = m_systemManager->GetSystem<ScriptingSystem>();
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
    ret = m_clientComponent.m_replicationManager.AddObserver(scriptingSystem);
    if (!ret) {
        return ret;
    }
    std::weak_ptr<SightSystemClient> sightSystemClient = m_systemManager->GetSystem<SightSystemClient>();
    ret = m_clientComponent.m_replicationManager.AddObserver(sightSystemClient);
    if (!ret) {
        return ret;
    }
#ifdef _DRAW
    std::weak_ptr<RendererSystem> rendererSystem = m_systemManager->GetSystem<RendererSystem>();
    ret = sightSystemClient.lock()->AddObserver(rendererSystem);
    if (!ret) {
        return ret;
    }
    ret = m_clientComponent.m_replicationManager.AddObserver(rendererSystem);
    if (!ret) {
        return ret;
    }
    std::weak_ptr<MovementSystemClient> movementSystemClient = m_systemManager->GetSystem<MovementSystemClient>();
    ret = movementSystemClient.lock()->AddObserver(rendererSystem);
    if (!ret) {
        return ret;
    }
    std::weak_ptr<RemotePlayerMovementSystem> remotePlayerMovementSystem = m_systemManager->GetSystem<RemotePlayerMovementSystem>();
    ret = remotePlayerMovementSystem.lock()->AddObserver(rendererSystem);
    if (!ret) {
        return ret;
    }
#endif

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool GameClient::DoFrame()
{
    bool ret = false;

    InitFrame();

    std::weak_ptr<ClientSystem> clientSystem = m_systemManager->GetSystem<ClientSystem>();
    clientSystem.lock()->ReceiveIncomingPackets(m_clientComponent);

    ret = Game::DoFrame();

    clientSystem.lock()->SendOutgoingPackets(m_clientComponent);

    EndFrame();

    return ret;
}
}
