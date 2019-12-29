#ifndef __GAME_SERVER_H__
#define __GAME_SERVER_H__

#include "Game.h"

#include "ServerComponent.h"

namespace sand {

struct ConfigServer;

//----------------------------------------------------------------------------------------------------
class GameServer : public Game {
public:
    GameServer(const char* configPath);

    bool Init() override;

    ServerComponent& GetServerComponent() { return m_serverComponent; }

private:
    std::shared_ptr<ConfigServer> m_configServer;

    ServerComponent m_serverComponent;
};

extern GameServer* g_gameServer;
}

#endif
