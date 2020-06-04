#ifndef __GAME_CLIENT_H__
#define __GAME_CLIENT_H__

#include "Game.h"

namespace sand {

struct ConfigClient;

struct ClientComponent;
struct ScriptingComponent;
struct InputComponent;

//----------------------------------------------------------------------------------------------------
class GameClient : public Game {
public:
    GameClient(const std::string& configPath);

    bool Init() override;
    bool DoFrame() override;

    std::weak_ptr<ClientComponent> GetClientComponent() { return std::weak_ptr(m_clientComponent); }
    std::weak_ptr<ScriptingComponent> GetScriptingComponent() { return std::weak_ptr(m_scriptingComponent); }
    std::weak_ptr<InputComponent> GetInputComponent() { return std::weak_ptr(m_inputComponent); }

private:
    std::shared_ptr<ConfigClient> m_configClient;

    std::shared_ptr<ClientComponent> m_clientComponent;
    std::shared_ptr<ScriptingComponent> m_scriptingComponent;
    std::shared_ptr<InputComponent> m_inputComponent;
};

extern GameClient* g_gameClient;
}

#endif
