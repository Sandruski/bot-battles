#include "HealthSystemServer.h"

#include "BotComponent.h"
#include "ClientProxy.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "EventComponent.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "HealthComponent.h"
#include "ServerComponent.h"
#include "SpriteComponent.h"
#include "State.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthSystemServer::HealthSystemServer()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::HEALTH);
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool HealthSystemServer::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool HealthSystemServer::Update()
{
    OPTICK_EVENT();

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.lock()->GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().GetComponent<SpriteComponent>(entity);
        std::weak_ptr<BotComponent> botComponent = g_gameServer->GetComponentManager().GetComponent<BotComponent>(entity);

        U64 characterDirtyState = 0;

        std::weak_ptr<ClientProxy> clientProxy = serverComponent.lock()->GetClientProxy(playerID);
        for (U32 i = clientProxy.lock()->m_inputBuffer.m_front; i < clientProxy.lock()->m_inputBuffer.m_back; ++i) {
            const bool canPerformAction = botComponent.lock()->CanPerformAction();
            if (!canPerformAction) {
                continue;
            }

            const Input& input = clientProxy.lock()->m_inputBuffer.Get(i);
            U64 dirtyState = input.GetDirtyState();

            // Heal
            const bool hasHeal = dirtyState & static_cast<U64>(InputComponentMemberType::INPUT_HEAL);
            if (hasHeal) {
                const bool result = healthComponent.lock()->CanHeal();
                if (!result) {
                    continue;
                }

                healthComponent.lock()->Heal();
                characterDirtyState |= static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HP);
                characterDirtyState |= static_cast<U64>(ComponentMemberType::HEALTH_HP);

                spriteComponent.lock()->m_spriteName = "heal";
                characterDirtyState |= static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME);

                botComponent.lock()->m_actionType = BotComponent::ActionType::HEAL;
                characterDirtyState |= static_cast<U64>(ComponentMemberType::BOT_ACTION_TYPE);
                botComponent.lock()->m_timeAction = healthComponent.lock()->m_timeHeal;
                characterDirtyState |= static_cast<U64>(ComponentMemberType::BOT_TIME_ACTION);
                botComponent.lock()->m_cooldownAction = healthComponent.lock()->m_cooldownHeal;
                botComponent.lock()->m_timerAction.Start();
            }
        }

        if (characterDirtyState > 0) {
            Event newEvent;
            newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
            newEvent.component.entity = entity;
            newEvent.component.dirtyState = characterDirtyState;
            NotifyEvent(newEvent);
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool HealthSystemServer::Render()
{
    OPTICK_EVENT();

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    for (const auto& entity : m_entities) {
        PlayerID playerID = serverComponent.lock()->GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entity);
        Draw(playerID, healthComponent);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void HealthSystemServer::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::COLLISION_ENTER: {
        OnCollisionEnter(event.collision.entityA, event.collision.entityB);
        break;
    }

    case EventType::WEAPON_HIT: {
        OnWeaponHit(event.weapon.shooterEntity, event.weapon.targetEntity, event.weapon.damage, event.weapon.direction);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void HealthSystemServer::OnCollisionEnter(Entity entityA, Entity entityB) const
{
    assert(entityA < INVALID_ENTITY && entityB < INVALID_ENTITY);

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    PlayerID playerIDA = serverComponent.lock()->GetPlayerID(entityA);
    PlayerID playerIDB = serverComponent.lock()->GetPlayerID(entityB);
    if (playerIDA >= INVALID_PLAYER_ID || playerIDB >= INVALID_PLAYER_ID) {
        return;
    }

    std::weak_ptr<HealthComponent> healthComponentA = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entityA);
    std::weak_ptr<HealthComponent> healthComponentB = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entityB);
    if (healthComponentA.expired() || healthComponentB.expired()) {
        return;
    }

    U64 characterDirtyState = 0;

    healthComponentA.lock()->m_currentHP = 0;
    characterDirtyState |= static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HP);
    healthComponentA.lock()->m_hitEntityLastShot = entityB;
    characterDirtyState |= static_cast<U64>(ComponentMemberType::HEALTH_HIT_ENTITY_LAST_SHOT);
    healthComponentB.lock()->m_currentHP = 0;
    healthComponentA.lock()->m_hitEntityLastShot = entityA;

    Event newHealthEvent;
    newHealthEvent.eventType = EventType::HEALTH_HURT;
    newHealthEvent.health.targetEntity = entityA;
    NotifyEvent(newHealthEvent);
    newHealthEvent.health.targetEntity = entityB;
    NotifyEvent(newHealthEvent);

    Event newComponentEvent;
    newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
    newComponentEvent.component.dirtyState = characterDirtyState;
    newComponentEvent.component.entity = entityA;
    NotifyEvent(newComponentEvent);
    newComponentEvent.component.entity = entityB;
    NotifyEvent(newComponentEvent);
}

//----------------------------------------------------------------------------------------------------
void HealthSystemServer::OnWeaponHit(Entity shooterEntity, Entity targetEntity, U32 damage, const glm::vec2& direction) const
{
    assert(shooterEntity < INVALID_ENTITY && targetEntity < INVALID_ENTITY);

    std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(targetEntity);
    if (healthComponent.expired()) {
        return;
    }

    U64 characterDirtyState = 0;

    I32 oldCurrentHP = healthComponent.lock()->m_currentHP;
    healthComponent.lock()->m_currentHP -= damage;
    if (healthComponent.lock()->m_currentHP <= 0) {
        healthComponent.lock()->m_currentHP = 0;
    }
    characterDirtyState |= static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HP);
    healthComponent.lock()->m_directionLastShot = direction;
    characterDirtyState |= static_cast<U64>(ComponentMemberType::HEALTH_DIRECTION_LAST_SHOT);
    healthComponent.lock()->m_hitEntityLastShot = shooterEntity;
    characterDirtyState |= static_cast<U64>(ComponentMemberType::HEALTH_HIT_ENTITY_LAST_SHOT);

    I32 currentHPDiff = oldCurrentHP - healthComponent.lock()->m_currentHP;
    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    PlayerID shooterPlayerID = serverComponent.lock()->GetPlayerID(shooterEntity);
    std::weak_ptr<ClientProxy> shooterClientProxy = serverComponent.lock()->GetClientProxy(shooterPlayerID);
    if (!shooterClientProxy.expired()) {
        shooterClientProxy.lock()->m_damageInflicted += currentHPDiff;
    }
    PlayerID targetPlayerID = serverComponent.lock()->GetPlayerID(targetEntity);
    std::weak_ptr<ClientProxy> targetClientProxy = serverComponent.lock()->GetClientProxy(targetPlayerID);
    if (!targetClientProxy.expired()) {
        targetClientProxy.lock()->m_damageReceived += currentHPDiff;
    }

    Event newHealthEvent;
    newHealthEvent.eventType = EventType::HEALTH_HURT;
    newHealthEvent.health.targetEntity = targetEntity;
    NotifyEvent(newHealthEvent);

    Event newComponentEvent;
    newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
    newComponentEvent.component.dirtyState = characterDirtyState;
    newComponentEvent.component.entity = targetEntity;
    NotifyEvent(newComponentEvent);
}
}
