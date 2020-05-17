#include "HealthSystem.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "HealthComponent.h"
#include "SpriteComponent.h"
#include "State.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthSystem::HealthSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::HEALTH);
    m_signature |= 1 << static_cast<U16>(ComponentType::SPRITE);
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool HealthSystem::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool HealthSystem::Update()
{
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    EventComponent& eventComponent = g_game->GetEventComponent();
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_LSHIFT) == EventComponent::KeyState::REPEAT
        && eventComponent.m_keyboard.at(SDL_SCANCODE_W) == EventComponent::KeyState::DOWN) {
        for (auto& entity : m_entities) {
            std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entity);
            if (healthComponent.expired() || !healthComponent.lock()->m_isEnabled) {
                continue;
            }

            healthComponent.lock()->m_health = 0;
        }

        for (auto& entity : m_entities) {
            std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entity);
            if (healthComponent.expired() || !healthComponent.lock()->m_isEnabled) {
                continue;
            }

            Event newHealthEvent;
            newHealthEvent.eventType = EventType::HEALTH_EMPTIED;
            newHealthEvent.health.entity = entity;
            NotifyEvent(newHealthEvent);
        }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
void HealthSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::COLLISION_ENTER: {
        OnCollisionEnter(event.collision.entityA, event.collision.entityB, event.collision.normal);
        break;
    }

    case EventType::WEAPON_HIT: {
        OnWeaponHit(event.weapon.entity, event.weapon.damage);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void HealthSystem::OnCollisionEnter(Entity entityA, Entity entityB, glm::vec2 /*normal*/) const
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    PlayerID playerIDA = serverComponent.GetPlayerID(entityA);
    PlayerID playerIDB = serverComponent.GetPlayerID(entityB);
    if (playerIDA >= INVALID_PLAYER_ID || playerIDB >= INVALID_PLAYER_ID) {
        return;
    }

    std::weak_ptr<HealthComponent> healthComponentA = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entityA);
    std::weak_ptr<HealthComponent> healthComponentB = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entityB);
    if (healthComponentA.expired() || healthComponentB.expired()) {
        return;
    }
    if (!healthComponentA.lock()->m_isEnabled || !healthComponentB.lock()->m_isEnabled) {
        return;
    }

    healthComponentA.lock()->m_health = 0;
    healthComponentB.lock()->m_health = 0;

    Event newHealthEvent;
    newHealthEvent.eventType = EventType::HEALTH_EMPTIED;
    newHealthEvent.health.entity = entityA;
    NotifyEvent(newHealthEvent);
    newHealthEvent.health.entity = entityB;
    NotifyEvent(newHealthEvent);

    Event newComponentEvent;
    newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
    newComponentEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::HEALTH_HEALTH);
    newComponentEvent.component.entity = entityA;
    NotifyEvent(newComponentEvent);
    newComponentEvent.component.entity = entityB;
    NotifyEvent(newComponentEvent);
}

//----------------------------------------------------------------------------------------------------
void HealthSystem::OnWeaponHit(Entity entity, U32 damage) const
{
    std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entity);
    if (healthComponent.expired() || !healthComponent.lock()->m_isEnabled) {
        return;
    }

    healthComponent.lock()->m_health -= damage;
    if (healthComponent.lock()->m_health <= 0) {
        healthComponent.lock()->m_health = 0;

        Event newHealthEvent;
        newHealthEvent.eventType = EventType::HEALTH_EMPTIED;
        newHealthEvent.health.entity = entity;
        NotifyEvent(newHealthEvent);
    }

    Event newComponentEvent;
    newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
    newComponentEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::HEALTH_HEALTH);
    newComponentEvent.component.entity = entity;
    NotifyEvent(newComponentEvent);
}
}
