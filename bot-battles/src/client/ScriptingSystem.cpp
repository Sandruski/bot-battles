#include "ScriptingSystem.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "RigidbodyComponent.h"
#include "ScriptingComponent.h"
#include "State.h"
#include "TransformComponent.h"
#include "WallComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ScriptingSystem::ScriptingSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::TRANSFORM);
    m_signature |= 1 << static_cast<U16>(ComponentType::RIGIDBODY);
    m_signature |= 1 << static_cast<U16>(ComponentType::LOCAL_PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool ScriptingSystem::StartUp()
{
    std::string scriptsDir = "import sys\n";
    scriptsDir.append("sys.path.append(\"");
    scriptsDir.append(BOTS_SCRIPTS_DIR);
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
bool ScriptingSystem::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ScriptingSystem::Update()
{
    OPTICK_EVENT();

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
        scriptingComponent.m_mainModule.attr("tick")(&inputComponent);
        scriptingComponent.m_mainModule.attr("log")();
    } catch (const std::runtime_error& /*re*/) {
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnNotify(const Event& event)
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

    case EventType::COLLISION_ENTER: {
        OnCollisionEnter(event.collision.entityA, event.collision.entityB, event.collision.linearVelocityA, event.collision.linearVelocityB, event.collision.normal);
        break;
    }

    case EventType::COLLISION_EXIT: {
        OnCollisionExit(event.collision.entityA, event.collision.entityB, event.collision.linearVelocityA, event.collision.linearVelocityB, event.collision.normal);
        break;
    }

        // TODO: onTriggerEnter & onTriggerExit for ammo pickups
        // TODO: network ammo tiles. What happens with current spawner component. Is it being networked?

    case EventType::SEEN_NEW_ENTITY: {
        OnSeenNewEntity(event.sight.entity);
        break;
    }

    case EventType::SEEN_LOST_ENTITY: {
        OnSeenLostEntity(event.sight.entity);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::ImportScripts() const
{
    Event newEvent;

    try {
        ScriptingComponent& scriptingComponent = g_gameClient->GetScriptingComponent();
        scriptingComponent.m_mainModule = py::module::import("main");
    } catch (const std::runtime_error& /*re*/) {
    }

    try {
        ScriptingComponent& scriptingComponent = g_gameClient->GetScriptingComponent();
        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        scriptingComponent.m_myBotModule = py::module::import(clientComponent.m_script.c_str());
        //scriptingComponent.m_myBotModule.reload(); // TODO

        newEvent.eventType = EventType::CONNECT_SUCCESSFUL;
    } catch (const std::runtime_error& re) {
        OutputDebugStringA(re.what());
        ::MessageBoxA(NULL, re.what(), "Error importing script", MB_OK);

        newEvent.eventType = EventType::CONNECT_FAILED;
    }

    NotifyEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::InitScripts() const
{
    ScriptingComponent& scriptingComponent = g_gameClient->GetScriptingComponent();
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameClient->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
        try {
            scriptingComponent.m_mainModule.attr("init")(clientComponent.m_script.c_str(), transformComponent.lock(), rigidbodyComponent.lock());
        } catch (const std::runtime_error& /*re*/) {
        }

        break;
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnCollisionEnter(Entity entityA, Entity entityB, glm::vec2 linearVelocityA, glm::vec2 linearVelocityB, glm::vec2 normal) const
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    Entity entity = INVALID_ENTITY;
    if (entityA == clientComponent.m_entity) {
        entity = entityB;
    } else if (entityB == clientComponent.m_entity) {
        entity = entityA;
    }

    if (entity >= INVALID_ENTITY) {
        return;
    }

    PhysicsComponent::Collision collision;
    collision.m_normal = normal;
    if (entity == entityA) {
        collision.m_relativeVelocity = linearVelocityA - linearVelocityB;
    } else if (entity == entityB) {
        collision.m_relativeVelocity = linearVelocityB - linearVelocityA;
    }

    std::weak_ptr<WallComponent> wallComponent = g_gameClient->GetComponentManager().GetComponent<WallComponent>(entity);
    if (!wallComponent.expired()) {
        ScriptingComponent& scriptingComponent = g_gameClient->GetScriptingComponent();
        InputComponent& inputComponent = g_gameClient->GetInputComponent();
        try {
            scriptingComponent.m_mainModule.attr("onHitWallEnter")(&inputComponent, collision);
            scriptingComponent.m_mainModule.attr("log")();
        } catch (const std::runtime_error& /*re*/) {
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnCollisionExit(Entity entityA, Entity entityB, glm::vec2 linearVelocityA, glm::vec2 linearVelocityB, glm::vec2 normal) const
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    Entity entity = INVALID_ENTITY;
    if (entityA == clientComponent.m_entity) {
        entity = entityB;
    } else if (entityB == clientComponent.m_entity) {
        entity = entityA;
    }

    if (entity >= INVALID_ENTITY) {
        return;
    }

    PhysicsComponent::Collision collision;
    collision.m_normal = normal;
    if (entity == entityA) {
        collision.m_relativeVelocity = linearVelocityA - linearVelocityB;
    } else if (entity == entityB) {
        collision.m_relativeVelocity = linearVelocityB - linearVelocityA;
    }

    std::weak_ptr<WallComponent> wallComponent = g_gameClient->GetComponentManager().GetComponent<WallComponent>(entity);
    if (!wallComponent.expired()) {
        ScriptingComponent& scriptingComponent = g_gameClient->GetScriptingComponent();
        InputComponent& inputComponent = g_gameClient->GetInputComponent();
        try {
            scriptingComponent.m_mainModule.attr("onHitWallExit")(&inputComponent, collision);
            scriptingComponent.m_mainModule.attr("log")();
        } catch (const std::runtime_error& /*re*/) {
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnSeenNewEntity(Entity /*entity*/) const
{
    // TODO: call different methods for different components
    ScriptingComponent& scriptingComponent = g_gameClient->GetScriptingComponent();
    InputComponent& inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.m_mainModule.attr("onSeenNewBot")(&inputComponent);
        scriptingComponent.m_mainModule.attr("log")();
    } catch (const std::runtime_error& /*re*/) {
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnSeenLostEntity(Entity /*entity*/) const
{
    // TODO: call different methods for different components
    ScriptingComponent& scriptingComponent = g_gameClient->GetScriptingComponent();
    InputComponent& inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.m_mainModule.attr("onSeenLostBot")(&inputComponent);
        scriptingComponent.m_mainModule.attr("log")();
    } catch (const std::runtime_error& /*re*/) {
    }
}
}
