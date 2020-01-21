#ifndef __GAME_CLIENT_H__
#define __GAME_CLIENT_H__

#include "ClientComponent.h"
#include "Game.h"

namespace sand {

struct ConfigClient;

//----------------------------------------------------------------------------------------------------
class GameClient : public Game {
public:
    GameClient(const std::string& configPath);

    bool Init() override;

    ClientComponent& GetClientComponent() { return m_clientComponent; }
    InputComponent& GetInputComponent() { return m_inputComponent; }

private:
    std::shared_ptr<ConfigClient> m_configClient;

    ClientComponent m_clientComponent;
    InputComponent m_inputComponent;
};

extern GameClient* g_gameClient;
}

#endif
