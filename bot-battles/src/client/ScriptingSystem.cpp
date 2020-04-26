#include "ScriptingSystem.h"

#include "ClientComponent.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "InputComponent.h"
#include "ScriptingComponent.h"
#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool ScriptingSystemClient::StartUp()
{
    py::initialize_interpreter();

    std::string scriptsDir = "import sys\n";
    scriptsDir.append("sys.path.append(\"");
    scriptsDir.append(SCRIPTS_DIR);
    scriptsDir.append("\")\n");
    py::exec(scriptsDir);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ScriptingSystemClient::Update()
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Play") {
        return true;
    }

    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    const bool hasPlayer = clientComponent.m_playerID < INVALID_PLAYER_ID;
    const bool hasEntity = clientComponent.m_entity < INVALID_ENTITY;
    if (!hasPlayer || !hasEntity) {
        return true;
    }

    ScriptingComponent& scriptingComponent = g_gameClient->GetScriptingComponent();
    InputComponent& inputComponent = g_gameClient->GetInputComponent();
    scriptingComponent.m_botModule.attr("tick")(&inputComponent);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ScriptingSystemClient::ShutDown()
{
    py::finalize_interpreter();

    return true;
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystemClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::TRY_CONNECT: {
        ImportModule();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystemClient::ImportModule() const
{
    ScriptingComponent& scriptingComponent = g_gameClient->GetScriptingComponent();
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    Event newEvent;
    try {
        scriptingComponent.m_botModule = py::module::import(clientComponent.m_script.c_str());
        newEvent.eventType = EventType::CONNECT_SUCCESSFUL;
    } catch (const std::runtime_error& /*re*/) {
        newEvent.eventType = EventType::CONNECT_FAILED;
    }
    NotifyEvent(newEvent);
}
}
