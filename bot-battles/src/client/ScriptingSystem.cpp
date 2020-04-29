#include "ScriptingSystem.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "ScriptingComponent.h"
#include "State.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ScriptingSystemClient::ScriptingSystemClient()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::LOCAL_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool ScriptingSystemClient::StartUp()
{
    std::string scriptsDir = "import sys\n";
    scriptsDir.append("sys.path.append(\"");
    scriptsDir.append(ROBOTS_SCRIPTS_DIR);
    scriptsDir.append("\")\n");
    py::exec(scriptsDir);

    std::string internalScriptsDir = "import sys\n";
    internalScriptsDir.append("sys.path.append(\"");
    internalScriptsDir.append(INTERNAL_SCRIPTS_DIR);
    internalScriptsDir.append("\")\n");
    py::exec(internalScriptsDir);

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
    try {
        scriptingComponent.m_mainModule.attr("update")(&inputComponent);
    } catch (const std::runtime_error& re) {
        ELOG("%s", re.what());
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystemClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::TRY_CONNECT: {
        ImportScripts();
        break;
    }

    case EventType::PLAYER_ENTITY_ADDED: {
        InitScripts();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystemClient::ImportScripts() const
{
    Event newEvent;

    try {
        ScriptingComponent& scriptingComponent = g_gameClient->GetScriptingComponent();
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        scriptingComponent.m_myBotModule = py::module::import(clientComponent.m_script.c_str());
        scriptingComponent.m_mainModule = py::module::import("main");

        newEvent.eventType = EventType::CONNECT_SUCCESSFUL;
    } catch (const std::runtime_error& re) {
        newEvent.eventType = EventType::CONNECT_FAILED;
        ELOG("%s", re.what());
    }

    NotifyEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystemClient::InitScripts() const
{
    ScriptingComponent& scriptingComponent = g_gameClient->GetScriptingComponent();
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        try {
            scriptingComponent.m_mainModule.attr("init")(clientComponent.m_script.c_str(), transformComponent.lock());
        } catch (const std::runtime_error& re) {
            ELOG("%s", re.what());
        }

        break;
    }
}
}
