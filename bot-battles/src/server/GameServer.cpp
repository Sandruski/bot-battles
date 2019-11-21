#include "GameServer.h"

#include "EntityManager.h"
#include "SystemManager.h"

#include "InputSystemServer.h"
#include "NavigationSystem.h"
#include "ServerSystem.h"
#include "SpawnerSystem.h"

#include "SingletonServerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GameServer::GameServer(const GameConfiguration& configuration)
    : Game(configuration)
{
    m_singletonServerComponent = std::make_shared<SingletonServerComponent>();
}

//----------------------------------------------------------------------------------------------------
GameServer::~GameServer()
{
}

//----------------------------------------------------------------------------------------------------
bool GameServer::Init()
{
    bool ret = m_systemManager->RegisterSystem<ServerSystem>();
    if (!ret) {
        return false;
    }
    ret = m_systemManager->RegisterSystem<SpawnerSystem>();
    if (!ret) {
        return false;
    }
    ret = m_systemManager->RegisterSystem<InputSystemServer>();
    if (!ret) {
        return false;
    }
    ret = m_systemManager->RegisterSystem<NavigationSystem>();
    if (!ret) {
        return false;
    }

    std::shared_ptr<ServerSystem> server = m_systemManager->GetSystem<ServerSystem>();
    ret = m_entityManager->AddObserver(server);
    if (!ret) {
        return false;
    }

    ret = server->AddObserver(server);
    if (!ret) {
        return false;
    }

    std::shared_ptr<SpawnerSystem> spawner = m_systemManager->GetSystem<SpawnerSystem>();
    ret = server->AddObserver(spawner);
    if (!ret) {
        return false;
    }

    std::shared_ptr<NavigationSystem> navigation = m_systemManager->GetSystem<NavigationSystem>();
    ret = navigation->AddObserver(server);
    if (!ret) {
        return false;
    }

    ret = Game::Init();

    return ret;
}
}
