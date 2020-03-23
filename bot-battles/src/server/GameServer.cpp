#include "GameServer.h"

#include "CollisionSystemServer.h"
#include "ConfigServer.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameplayStateServer.h"
#include "HealthSystem.h"
#include "InputSystemServer.h"
#include "LinkingContext.h"
#include "MainMenuStateServer.h"
#include "MovementSystemServer.h"
#include "ServerComponent.h"
#include "ServerSystem.h"
#include "SpawnerSystem.h"
#include "SystemManager.h"
#include "WeaponSystemServer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GameServer::GameServer(const std::string& configPath)
    : m_configServer(nullptr)
    , m_serverComponent()
{
    m_config = m_configServer = std::make_shared<ConfigServer>(configPath);
}

//----------------------------------------------------------------------------------------------------
bool GameServer::Init()
{
    bool ret = false;

    // Systems
    ret = m_systemManager->RegisterSystem<ServerSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<SpawnerSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<MovementSystemServer>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<CollisionSystemServer>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<WeaponSystemServer>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<HealthSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<InputSystemServer>();
    if (!ret) {
        return ret;
    }

    // States
    ret = m_fsm->RegisterState<MainMenuStateServer>();
    if (!ret) {
        return ret;
    }
    ret = m_fsm->RegisterState<GameplayStateServer>();
    if (!ret) {
        return ret;
    }

    std::weak_ptr<ServerSystem> serverSystem = m_systemManager->GetSystem<ServerSystem>();
    ret = m_linkingContext->AddObserver(serverSystem);
    if (!ret) {
        return ret;
    }
    std::weak_ptr<SpawnerSystem> spawnerSystem = m_systemManager->GetSystem<SpawnerSystem>();
    ret = serverSystem.lock()->AddObserver(spawnerSystem);
    if (!ret) {
        return ret;
    }
    ret = serverSystem.lock()->AddObserver(serverSystem);
    if (!ret) {
        return ret;
    }
    ret = serverSystem.lock()->AddObserver(std::weak_ptr<Observer>(m_fsm));
    if (!ret) {
        return ret;
    }
    std::weak_ptr<MovementSystemServer> movementSystemServer = m_systemManager->GetSystem<MovementSystemServer>();
    ret = movementSystemServer.lock()->AddObserver(serverSystem);
    if (!ret) {
        return ret;
    }
    std::weak_ptr<WeaponSystemServer> weaponSystemServer = m_systemManager->GetSystem<WeaponSystemServer>();
    ret = weaponSystemServer.lock()->AddObserver(serverSystem);
    if (!ret) {
        return ret;
    }
    std::weak_ptr<HealthSystem> healthSystem = m_systemManager->GetSystem<HealthSystem>();
    ret = healthSystem.lock()->AddObserver(serverSystem);
    if (!ret) {
        return ret;
    }

    ret = Game::Init();

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool GameServer::Update()
{
    bool ret = false;

    std::weak_ptr<ServerSystem> serverSystem = m_systemManager->GetSystem<ServerSystem>();

    if (m_mainMenuComponent.m_phase != MainMenuComponent::MainMenuPhase::SETUP) {
        serverSystem.lock()->ReceiveIncomingPackets(m_serverComponent);
    }

    ret = Game::Update();

    if (m_mainMenuComponent.m_phase != MainMenuComponent::MainMenuPhase::SETUP) {
        serverSystem.lock()->SendOutgoingPackets(m_serverComponent);
    }

    return ret;
}
}
