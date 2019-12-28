#ifndef __GAME_CLIENT_H__
#define __GAME_CLIENT_H__

#include "Game.h"

#include "DeliveryManagerClient.h"
#include "ReplicationManagerClient.h"

#include "ClientComponent.h"
#include "MoveComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class GameClient : public Game {
public:
    GameClient(const GameConfiguration& configuration);

    bool Init() override;

    ReplicationManagerClient& GetReplicationManager() { return m_replicationManager; }
    DeliveryManagerClient& GetDeliveryManager() { return m_deliveryManager; }

    MoveComponent& GetMoveComponent() { return m_moveComponent; }
    ClientComponent& GetClientComponent() { return m_clientComponent; }

private:
    ReplicationManagerClient m_replicationManager;
    DeliveryManagerClient m_deliveryManager;

    MoveComponent m_moveComponent;
    ClientComponent m_clientComponent;
};

extern GameClient* g_gameClient;
}

#endif
