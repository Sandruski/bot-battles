#ifndef __GAME_CLIENT_H__
#define __GAME_CLIENT_H__

#include "Game.h"

namespace sand {

class ReplicationManagerClient;

struct SingletonInputComponent;
struct SingletonClientComponent;

//----------------------------------------------------------------------------------------------------
class GameClient : public Game {
public:
    GameClient(const GameConfiguration& configuration);
    ~GameClient() override;

    bool Init() override;

    ReplicationManagerClient& GetReplicationManager() const { return *m_replicationManager; }

    std::shared_ptr<SingletonInputComponent> GetSingletonInputComponent() const { return m_singletonInputComponent; }
    std::shared_ptr<SingletonClientComponent> GetSingletonClientComponent() const { return m_singletonClientComponent; }

private:
    std::unique_ptr<ReplicationManagerClient> m_replicationManager;

    std::shared_ptr<SingletonInputComponent> m_singletonInputComponent;
    std::shared_ptr<SingletonClientComponent> m_singletonClientComponent;
};

extern GameClient* g_gameClient;
}

#endif
