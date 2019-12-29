#ifndef __GAME_CLIENT_H__
#define __GAME_CLIENT_H__

#include "ClientComponent.h"
#include "DeliveryManagerClient.h"
#include "Game.h"
#include "MoveComponent.h"
#include "ReplicationManagerClient.h"

namespace sand {

struct ConfigClient;

//----------------------------------------------------------------------------------------------------
class GameClient : public Game {
public:
    GameClient(const char* configPath);

    bool Init() override;

    ReplicationManagerClient& GetReplicationManager() { return m_replicationManager; }
    DeliveryManagerClient& GetDeliveryManager() { return m_deliveryManager; }

    MoveComponent& GetMoveComponent() { return m_moveComponent; }
    ClientComponent& GetClientComponent() { return m_clientComponent; }

private:
    std::shared_ptr<ConfigClient> m_configClient;

    ReplicationManagerClient m_replicationManager;
    DeliveryManagerClient m_deliveryManager;

    MoveComponent m_moveComponent;
    ClientComponent m_clientComponent;
};

extern GameClient* g_gameClient;
}

#endif
