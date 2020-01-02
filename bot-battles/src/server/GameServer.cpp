#include "GameServer.h"

#include "ConfigServer.h"
#include "EntityManager.h"
#include "NavigationSystemServer.h"
#include "ServerComponent.h"
#include "ServerSystem.h"
#include "SpawnerSystem.h"
#include "SystemManager.h"

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

    std::weak_ptr<ServerSystem> serverSystem = m_systemManager->GetSystem<ServerSystem>();
    ret = m_entityManager->AddObserver(serverSystem);
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
    std::weak_ptr<NavigationSystemServer> navigationSystemServer = m_systemManager->GetSystem<NavigationSystemServer>();
    ret = navigationSystemServer.lock()->AddObserver(serverSystem);
    if (!ret) {
        return ret;
    }

    ret = Game::Init();

    return ret;
}
}
