#include "HealthSystemServer.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "HealthComponent.h"
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
            if (healthComponent.expired()) {
                continue;
            }

            healthComponent.lock()->m_currentHealth = 0;
        }

        for (auto& entity : m_entities) {
            std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entity);
            if (healthComponent.expired()) {
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
bool HealthSystemServer::Render()
{
    OPTICK_EVENT();

    RendererComponent& rendererComponent = g_gameServer->GetRendererComponent();
    for (const auto& entity : m_entities) {
        ServerComponent& serverComponent = g_gameServer->GetServerComponent();
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entity);

        glm::vec4 color = White;
        glm::vec4 backgroundColor = Black;
        backgroundColor.a = 0.5f;
        Draw(rendererComponent, transformComponent, healthComponent, color, backgroundColor);
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
        OnWeaponHit(event.weapon.entity, event.weapon.damage);
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

    healthComponentA.lock()->m_currentHealth = 0;
    healthComponentB.lock()->m_currentHealth = 0;

    Event newHealthEvent;
    newHealthEvent.eventType = EventType::HEALTH_EMPTIED;
    newHealthEvent.health.entity = entityA;
    NotifyEvent(newHealthEvent);
    newHealthEvent.health.entity = entityB;
    NotifyEvent(newHealthEvent);

    Event newComponentEvent;
    newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
    newComponentEvent.component.dirtyState = static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HEALTH);
    newComponentEvent.component.entity = entityA;
    NotifyEvent(newComponentEvent);
    newComponentEvent.component.entity = entityB;
    NotifyEvent(newComponentEvent);
}

//----------------------------------------------------------------------------------------------------
void HealthSystemServer::OnWeaponHit(Entity entity, U32 damage) const
{
    std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entity);
    if (healthComponent.expired()) {
        return;
    }

    healthComponent.lock()->m_currentHealth -= damage;
    if (healthComponent.lock()->m_currentHealth <= 0) {
        healthComponent.lock()->m_currentHealth = 0;

        Event newHealthEvent;
        newHealthEvent.eventType = EventType::HEALTH_EMPTIED;
        newHealthEvent.health.entity = entity;
        NotifyEvent(newHealthEvent);
    }

    Event newComponentEvent;
    newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
    newComponentEvent.component.dirtyState = static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HEALTH);
    newComponentEvent.component.entity = entity;
    NotifyEvent(newComponentEvent);
}
}