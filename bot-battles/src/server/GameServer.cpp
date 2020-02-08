#include "GameServer.h"

#include "CollisionSystem.h"
#include "ConfigServer.h"
#include "EntityManager.h"
#include "FSM.h"
#include "LinkingContext.h"
#include "NavigationSystemServer.h"
#include "ServerComponent.h"
#include "ServerSystem.h"
#include "SpawnerSystem.h"
#include "SystemManager.h"
#include "WeaponSystem.h"

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
    ret = m_systemManager->RegisterSystem<NavigationSystemServer>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<CollisionSystem>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<WeaponSystem>();
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
    std::weak_ptr<NavigationSystemServer> navigationSystemServer = m_systemManager->GetSystem<NavigationSystemServer>();
    ret = navigationSystemServer.lock()->AddObserver(serverSystem);
    if (!ret) {
        return ret;
    }
    std::weak_ptr<CollisionSystem> collisionSystem = m_systemManager->GetSystem<CollisionSystem>();
    ret = collisionSystem.lock()->AddObserver(serverSystem);
    if (!ret) {
        return ret;
    }

    ret = Game::Init();

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool GameServer::Update()
{
    std::weak_ptr<ServerSystem> serverSystem = m_systemManager->GetSystem<ServerSystem>();

    serverSystem.lock()->ReceiveIncomingPackets(m_serverComponent);

    Game::Update();

    serverSystem.lock()->SendOutgoingPackets(m_serverComponent);

    return true;
}
}
