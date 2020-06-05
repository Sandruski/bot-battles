#include "ScriptingSystem.h"

#include "BotComponent.h"
#include "ClientComponent.h"
#include "ComponentManager.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "HealthComponent.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "MapComponent.h"
#include "PhysicsComponent.h"
#include "RigidbodyComponent.h"
#include "ScriptingComponent.h"
#include "SpriteComponent.h"
#include "State.h"
#include "TransformComponent.h"
#include "WallComponent.h"
#include "WeaponComponent.h"

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

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired() || currentState.lock()->GetName() != "Play") {
        return true;
    }

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    const bool hasPlayer = clientComponent.lock()->m_playerID < INVALID_PLAYER_ID;
    const bool hasEntity = clientComponent.lock()->m_entity < INVALID_ENTITY;
    if (!hasPlayer || !hasEntity) {
        return true;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("tick")(inputComponent.lock());
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& re) {
        OutputDebugStringA(re.what());
        ::MessageBoxA(NULL, re.what(), "Error updating script", MB_OK);

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

    case EventType::SEEN_NEW_ENTITY: {
        OnSeenNewEntity(event.sight.entity);
        break;
    }

    case EventType::SEEN_LOST_ENTITY: {
        OnSeenLostEntity(event.sight.entity);
        break;
    }

    case EventType::WEAPON_HIT: {
        OnWeaponHit(event.weapon.entity);
        break;
    }

    case EventType::WEAPON_MISSED: {
        OnWeaponMissed(event.weapon.entity);
        break;
    }

    case EventType::WEAPON_PRIMARY_GAINED: {
        OnWeaponPrimaryGained(event.weapon.entity);
        break;
    }

    case EventType::HEALTH_LOST: {
        OnHealthLost(event.health.entity, event.health.health);
        break;
    }

    case EventType::HEALTH_GAINED: {
        OnHealthGained(event.health.entity, event.health.health);
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
        std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
        scriptingComponent.lock()->m_mainModule = py::module::import("main");
    } catch (const std::runtime_error& /*re*/) {
    }

    try {
        std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
        std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
        scriptingComponent.lock()->m_myBotModule = py::module::import(clientComponent.lock()->m_bot.c_str());
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
    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();

    for (auto& entity : m_entities) {
        if (g_gameClient->GetLinkingContext().GetNetworkID(entity) >= INVALID_NETWORK_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<RigidbodyComponent> rigidbodyComponent = g_gameClient->GetComponentManager().GetComponent<RigidbodyComponent>(entity);
        std::weak_ptr<HealthComponent> healthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(entity);
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
        std::weak_ptr<MapComponent> mapComponent = g_gameClient->GetMapComponent();
        try {
            scriptingComponent.lock()->m_mainModule.attr("init")(clientComponent.lock()->m_bot.c_str(), transformComponent.lock(), rigidbodyComponent.lock(), healthComponent.lock(), weaponComponent.lock(), mapComponent.lock());
        } catch (const std::runtime_error& re) {
            OutputDebugStringA(re.what());
            ::MessageBoxA(NULL, re.what(), "Error initializing script", MB_OK);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnCollisionEnter(Entity entityA, Entity entityB, glm::vec2 linearVelocityA, glm::vec2 linearVelocityB, glm::vec2 normal) const
{
    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    Entity entity = INVALID_ENTITY;
    if (entityA == clientComponent.lock()->m_entity) {
        entity = entityB;
    } else if (entityB == clientComponent.lock()->m_entity) {
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
        std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
        std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
        try {
            scriptingComponent.lock()->m_mainModule.attr("onHitWall")(inputComponent.lock(), collision);
            scriptingComponent.lock()->m_mainModule.attr("log")();
        } catch (const std::runtime_error& re) {
            OutputDebugStringA(re.what());
            ::MessageBoxA(NULL, re.what(), "Error OnCollisionEnter", MB_OK);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnSeenNewEntity(Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (entity == clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();

    std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(entity);
    if (!botComponent.expired()) {
        try {
            scriptingComponent.lock()->m_mainModule.attr("onSeenNewBot")(inputComponent.lock());
            scriptingComponent.lock()->m_mainModule.attr("log")();
        } catch (const std::runtime_error& re) {
            OutputDebugStringA(re.what());
            ::MessageBoxA(NULL, re.what(), "Error OnSeenNewEntity", MB_OK);
        }
        return;
    }

    std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
    if (!botComponent.expired()) {
        try {
            scriptingComponent.lock()->m_mainModule.attr("onSeenNewWeapon")(inputComponent.lock());
            scriptingComponent.lock()->m_mainModule.attr("log")();
        } catch (const std::runtime_error& re) {
            OutputDebugStringA(re.what());
            ::MessageBoxA(NULL, re.what(), "Error OnSeenNewEntity", MB_OK);
        }
        return;
    }

    std::weak_ptr<HealthComponent> healthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(entity);
    if (!botComponent.expired()) {
        try {
            scriptingComponent.lock()->m_mainModule.attr("onSeenNewHealth")(inputComponent.lock());
            scriptingComponent.lock()->m_mainModule.attr("log")();
        } catch (const std::runtime_error& re) {
            OutputDebugStringA(re.what());
            ::MessageBoxA(NULL, re.what(), "Error OnSeenNewEntity", MB_OK);
        }
        return;
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnSeenLostEntity(Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (entity == clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();

    std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(entity);
    if (!botComponent.expired()) {
        try {
            scriptingComponent.lock()->m_mainModule.attr("onSeenLostBot")(inputComponent.lock());
            scriptingComponent.lock()->m_mainModule.attr("log")();
        } catch (const std::runtime_error& re) {
            OutputDebugStringA(re.what());
            ::MessageBoxA(NULL, re.what(), "Error OnSeenLostEntity", MB_OK);
        }
    }

    std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
    if (!botComponent.expired()) {
        try {
            scriptingComponent.lock()->m_mainModule.attr("onSeenLostWeapon")(inputComponent.lock());
            scriptingComponent.lock()->m_mainModule.attr("log")();
        } catch (const std::runtime_error& re) {
            OutputDebugStringA(re.what());
            ::MessageBoxA(NULL, re.what(), "Error OnSeenLostEntity", MB_OK);
        }
        return;
    }

    std::weak_ptr<HealthComponent> healthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(entity);
    if (!botComponent.expired()) {
        try {
            scriptingComponent.lock()->m_mainModule.attr("onSeenLostHealth")(inputComponent.lock());
            scriptingComponent.lock()->m_mainModule.attr("log")();
        } catch (const std::runtime_error& re) {
            OutputDebugStringA(re.what());
            ::MessageBoxA(NULL, re.what(), "Error OnSeenLostEntity", MB_OK);
        }
        return;
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnWeaponHit(Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (entity != clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("onBulletHit")(inputComponent.lock());
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& re) {
        OutputDebugStringA(re.what());
        ::MessageBoxA(NULL, re.what(), "Error OnWeaponHit", MB_OK);
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnWeaponMissed(Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (entity != clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("onBulletMiss")(inputComponent.lock());
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& re) {
        OutputDebugStringA(re.what());
        ::MessageBoxA(NULL, re.what(), "Error OnWeaponMissed", MB_OK);
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnWeaponPrimaryGained(Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (entity != clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("onWeaponPicked")(inputComponent.lock());
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& re) {
        OutputDebugStringA(re.what());
        ::MessageBoxA(NULL, re.what(), "Error OnWeaponPrimaryGained", MB_OK);
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnHealthLost(Entity entity, U32 /*health*/) const
{
    // TODO: do smth with health

    assert(entity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (entity != clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("onHitByBullet")(inputComponent.lock());
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& re) {
        OutputDebugStringA(re.what());
        ::MessageBoxA(NULL, re.what(), "Error OnHealthLost", MB_OK);
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnHealthGained(Entity entity, U32 /*health*/) const
{
    // TODO: do smth with health

    assert(entity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (entity != clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("onHealthPicked")(inputComponent.lock());
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& re) {
        OutputDebugStringA(re.what());
        ::MessageBoxA(NULL, re.what(), "Error OnHealthGained", MB_OK);
    }
}
}
