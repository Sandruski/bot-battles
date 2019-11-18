#ifndef __GAME_SERVER_H__
#define __GAME_SERVER_H__

#include "Game.h"

namespace sand {

struct SingletonServerComponent;

//----------------------------------------------------------------------------------------------------
class GameServer : public Game {
public:
    GameServer(const GameConfiguration& configuration);
    ~GameServer() override;

    bool Init() override;

    std::shared_ptr<SingletonServerComponent> GetSingletonServerComponent() const { return m_singletonServerComponent; }

private:
    std::shared_ptr<SingletonServerComponent> m_singletonServerComponent;
};

extern GameServer* g_gameServer;
}

#endif
