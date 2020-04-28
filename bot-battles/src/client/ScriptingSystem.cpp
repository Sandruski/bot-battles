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

    try {
        scriptingComponent.m_botModule.attr("update")(&inputComponent);
    } catch (const std::runtime_error& re) {
        OutputDebugStringA(re.what());
        ::MessageBoxA(NULL, re.what(), "Error initializing sample", MB_OK);
    }

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
        ImportScript();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystemClient::ImportScript() const
{
    Event newEvent;
    newEvent.eventType = EventType::CONNECT_FAILED;

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        try {
            ScriptingComponent& scriptingComponent = g_gameClient->GetScriptingComponent();
            ClientComponent& clientComponent = g_gameClient->GetClientComponent();
            scriptingComponent.m_botModule = py::module::import(clientComponent.m_script.c_str());

            std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
            scriptingComponent.m_botModule.attr("init")(transformComponent.lock());

            newEvent.eventType = EventType::CONNECT_SUCCESSFUL;
        } catch (const std::runtime_error& /*re*/) {
        }

        break;
    }

    NotifyEvent(newEvent);
}
}
