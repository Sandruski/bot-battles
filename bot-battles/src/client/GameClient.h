#ifndef __GAME_CLIENT_H__
#define __GAME_CLIENT_H__

#include "ClientComponent.h"
#include "Game.h"
#include "InputComponent.h"
#include "ScriptingComponent.h"

namespace sand {

struct ConfigClient;

//----------------------------------------------------------------------------------------------------
class GameClient : public Game {
public:
    GameClient(const std::string& configPath);

    bool Init() override;
    bool DoFrame() override;

    ClientComponent& GetClientComponent() { return m_clientComponent; }
    ScriptingComponent& GetScriptingComponent() { return m_scriptingComponent; }
    InputComponent& GetInputComponent() { return m_inputComponent; }

private:
    std::shared_ptr<ConfigClient> m_configClient;

    ClientComponent m_clientComponent;
    ScriptingComponent m_scriptingComponent;
    InputComponent m_inputComponent;
};

extern GameClient* g_gameClient;
}

#endif
