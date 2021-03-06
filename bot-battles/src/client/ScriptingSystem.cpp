#include "ScriptingSystem.h"

#include "BotComponent.h"
#include "ClientComponent.h"
#include "ColliderComponent.h"
#include "ComponentManager.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "HealthComponent.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "MapComponent.h"
#include "PhysicsComponent.h"
#include "PlayerComponent.h"
#include "RigidbodyComponent.h"
#include "ScriptingComponent.h"
#include "SightComponent.h"
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
    std::string botsScriptsDir = "import sys\n";
    botsScriptsDir.append("sys.path.append(\"");
    botsScriptsDir.append(BOTS_SCRIPTS_DIR);
    botsScriptsDir.append("\")\n");
    py::exec(botsScriptsDir);

    std::string botsOtherScriptsDir = "import sys\n";
    botsOtherScriptsDir.append("sys.path.append(\"");
    botsOtherScriptsDir.append(BOTS_OTHER_SCRIPTS_DIR);
    botsOtherScriptsDir.append("\")\n");
    py::exec(botsOtherScriptsDir);

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
        OnSeenNewEntity(event.sight.seenEntity);
        break;
    }

    case EventType::SEEN_LOST_ENTITY: {
        OnSeenLostEntity(event.sight.seenEntity);
        break;
    }

    case EventType::WEAPON_HIT: {
        OnWeaponHit(event.weapon.shooterEntity);
        break;
    }

    case EventType::WEAPON_MISSED: {
        OnWeaponMissed(event.weapon.shooterEntity);
        break;
    }

    case EventType::WEAPON_PRIMARY_PICKED_UP: {
        OnWeaponPrimaryPickedUp(event.weapon.shooterEntity);
        break;
    }

    case EventType::WEAPON_PRIMARY_RELOADED: {
        OnWeaponPrimaryReloaded(event.weapon.shooterEntity, event.weapon.ammo);
        break;
    }

    case EventType::HEALTH_HURT: {
        OnHealthHurt(event.health.targetEntity, event.health.health, event.health.direction);
        break;
    }

    case EventType::HEALTH_HEALED: {
        OnHealthHealed(event.health.targetEntity, event.health.health);
        break;
    }

    case EventType::HEALTH_PICKED_UP: {
        OnHealthPickedUp(event.health.targetEntity);
        break;
    }

    case EventType::KILL: {
        OnKill(event.entity.entity);
        break;
    }

    case EventType::DEATH: {
        OnDeath(event.entity.entity);
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
        scriptingComponent.lock()->m_myBotModule.reload();

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
        std::weak_ptr<ColliderComponent> colliderComponent = g_gameClient->GetComponentManager().GetComponent<ColliderComponent>(entity);
        std::weak_ptr<HealthComponent> healthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(entity);
        std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(entity);
        std::weak_ptr<SightComponent> sightComponent = g_gameClient->GetComponentManager().GetComponent<SightComponent>(entity);
        std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(entity);
        std::weak_ptr<MapComponent> mapComponent = g_gameClient->GetMapComponent();
        try {
            scriptingComponent.lock()->m_mainModule.attr("init")(clientComponent.lock()->m_bot.c_str(), clientComponent.lock()->m_playerID, transformComponent.lock(), rigidbodyComponent.lock(), colliderComponent.lock(), weaponComponent.lock(), healthComponent.lock(), sightComponent.lock(), botComponent.lock(), mapComponent.lock());
        } catch (const std::runtime_error& re) {
            OutputDebugStringA(re.what());
            ::MessageBoxA(NULL, re.what(), "Error initializing script", MB_OK);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnCollisionEnter(Entity entityA, Entity entityB, const glm::vec2& linearVelocityA, const glm::vec2& linearVelocityB, const glm::vec2& normal) const
{
    assert(entityA < INVALID_ENTITY && entityB < INVALID_ENTITY);

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
        } catch (const std::runtime_error& /*re*/) {
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnSeenNewEntity(Entity seenEntity) const
{
    assert(seenEntity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (seenEntity == clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();

    std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(seenEntity);
    if (!botComponent.expired()) {
        try {
            scriptingComponent.lock()->m_mainModule.attr("onSeenNewBot")(inputComponent.lock(), seenEntity);
            scriptingComponent.lock()->m_mainModule.attr("log")();
        } catch (const std::runtime_error& /*re*/) {
        }
        return;
    }

    std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(seenEntity);
    if (!botComponent.expired()) {
        try {
            scriptingComponent.lock()->m_mainModule.attr("onSeenNewWeapon")(inputComponent.lock(), seenEntity);
            scriptingComponent.lock()->m_mainModule.attr("log")();
        } catch (const std::runtime_error& /*re*/) {
        }
        return;
    }

    std::weak_ptr<HealthComponent> healthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(seenEntity);
    if (!botComponent.expired()) {
        try {
            scriptingComponent.lock()->m_mainModule.attr("onSeenNewHealth")(inputComponent.lock(), seenEntity);
            scriptingComponent.lock()->m_mainModule.attr("log")();
        } catch (const std::runtime_error& /*re*/) {
        }
        return;
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnSeenLostEntity(Entity seenEntity) const
{
    assert(seenEntity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (seenEntity == clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();

    std::weak_ptr<BotComponent> botComponent = g_gameClient->GetComponentManager().GetComponent<BotComponent>(seenEntity);
    if (!botComponent.expired()) {
        try {
            scriptingComponent.lock()->m_mainModule.attr("onSeenLostBot")(inputComponent.lock(), seenEntity);
            scriptingComponent.lock()->m_mainModule.attr("log")();
        } catch (const std::runtime_error& /*re*/) {
        }
    }

    std::weak_ptr<WeaponComponent> weaponComponent = g_gameClient->GetComponentManager().GetComponent<WeaponComponent>(seenEntity);
    if (!botComponent.expired()) {
        try {
            scriptingComponent.lock()->m_mainModule.attr("onSeenLostWeapon")(inputComponent.lock(), seenEntity);
            scriptingComponent.lock()->m_mainModule.attr("log")();
        } catch (const std::runtime_error& /*re*/) {
        }
        return;
    }

    std::weak_ptr<HealthComponent> healthComponent = g_gameClient->GetComponentManager().GetComponent<HealthComponent>(seenEntity);
    if (!botComponent.expired()) {
        try {
            scriptingComponent.lock()->m_mainModule.attr("onSeenLostHealth")(inputComponent.lock(), seenEntity);
            scriptingComponent.lock()->m_mainModule.attr("log")();
        } catch (const std::runtime_error& /*re*/) {
        }
        return;
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnWeaponHit(Entity shooterEntity) const
{
    assert(shooterEntity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (shooterEntity != clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("onBulletHit")(inputComponent.lock());
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& /*re*/) {
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnWeaponMissed(Entity shooterEntity) const
{
    assert(shooterEntity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (shooterEntity != clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("onBulletMiss")(inputComponent.lock());
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& /*re*/) {
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnWeaponPrimaryPickedUp(Entity shooterEntity) const
{
    assert(shooterEntity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (shooterEntity != clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("onWeaponPickedUp")(inputComponent.lock());
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& /*re*/) {
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnWeaponPrimaryReloaded(Entity shooterEntity, U32 ammo) const
{
    assert(shooterEntity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (shooterEntity != clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("onReloaded")(inputComponent.lock(), ammo);
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& /*re*/) {
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnHealthHurt(Entity targetEntity, U32 health, const glm::vec2& direction) const
{
    assert(targetEntity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (targetEntity != clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("onHitByBullet")(inputComponent.lock(), health, std::tuple<F32, F32>(direction.x, direction.y));
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& /*re*/) {
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnHealthHealed(Entity targetEntity, U32 health) const
{
    assert(targetEntity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (targetEntity != clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("onHealed")(inputComponent.lock(), health);
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& /*re*/) {
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnHealthPickedUp(Entity targetEntity) const
{
    assert(targetEntity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (targetEntity != clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    std::weak_ptr<InputComponent> inputComponent = g_gameClient->GetInputComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("onHealthPickedUp")(inputComponent.lock());
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& /*re*/) {
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnKill(Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (entity != clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("onKill")();
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& /*re*/) {
    }
}

//----------------------------------------------------------------------------------------------------
void ScriptingSystem::OnDeath(Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    if (entity != clientComponent.lock()->m_entity) {
        return;
    }

    std::weak_ptr<ScriptingComponent> scriptingComponent = g_gameClient->GetScriptingComponent();
    try {
        scriptingComponent.lock()->m_mainModule.attr("onDeath")();
        scriptingComponent.lock()->m_mainModule.attr("log")();
    } catch (const std::runtime_error& /*re*/) {
    }
}
}
