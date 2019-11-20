#include "GameClient.h"

#include "DeliveryManager.h"
#include "ReplicationManagerClient.h"
#include "SystemManager.h"

#include "ClientSystem.h"
#include "InputSystemClient.h"

#include "SingletonClientComponent.h"
#include "SingletonInputComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GameClient::GameClient(const GameConfiguration& configuration)
    : Game(configuration)
    , m_replicationManager()
    , m_singletonInputComponent()
    , m_singletonClientComponent()
{
    m_replicationManager = std::make_unique<ReplicationManagerClient>();
    m_deliveryManager = std::make_unique<DeliveryManager>();

    m_singletonInputComponent = std::make_shared<SingletonInputComponent>();
    m_singletonClientComponent = std::make_shared<SingletonClientComponent>();
}

//----------------------------------------------------------------------------------------------------
GameClient::~GameClient()
{
}

//----------------------------------------------------------------------------------------------------
bool GameClient::Init()
{
    bool ret = m_systemManager->RegisterSystem<InputSystemClient>();
    if (!ret) {
        return false;
    }
    ret = m_systemManager->RegisterSystem<ClientSystem>();
    if (!ret) {
        return false;
    }

    ret = Game::Init();

    return ret;
}
}
