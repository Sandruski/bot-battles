#include "GameServer.h"

#include "ConfigServer.h"
#include "EntityManager.h"
#include "InputSystemServer.h"
#include "NavigationSystem.h"
#include "ServerComponent.h"
#include "ServerSystem.h"
#include "SpawnerSystem.h"
#include "SystemManager.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GameServer::GameServer(const char* configPath)
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
    ret = m_systemManager->RegisterSystem<InputSystemServer>();
    if (!ret) {
        return ret;
    }
    ret = m_systemManager->RegisterSystem<NavigationSystem>();
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
    std::weak_ptr<NavigationSystem> navigationSystem = m_systemManager->GetSystem<NavigationSystem>();
    ret = navigationSystem.lock()->AddObserver(serverSystem);
    if (!ret) {
        return ret;
    }

    ret = Game::Init();

    return ret;
}
}
