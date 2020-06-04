#ifndef __GAME_SERVER_H__
#define __GAME_SERVER_H__

#include "Game.h"

namespace sand {

struct ConfigServer;

struct ServerComponent;

//----------------------------------------------------------------------------------------------------
class GameServer : public Game {
public:
    GameServer(const std::string& configPath);

    bool Init() override;
    bool DoFrame() override;

    std::weak_ptr<ServerComponent> GetServerComponent() { return std::weak_ptr(m_serverComponent); }

private:
    std::shared_ptr<ConfigServer> m_configServer;

    std::shared_ptr<ServerComponent> m_serverComponent;
};

extern GameServer* g_gameServer;
}

#endif
