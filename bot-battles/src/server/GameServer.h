#ifndef __GAME_SERVER_H__
#define __GAME_SERVER_H__

#include "Game.h"

#include "ServerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class GameServer : public Game {
public:
    GameServer(const GameConfiguration& configuration);

    bool Init() override;

    ServerComponent& GetServerComponent() { return m_serverComponent; }

private:
    ServerComponent m_serverComponent;
};

extern GameServer* g_gameServer;
}

#endif
