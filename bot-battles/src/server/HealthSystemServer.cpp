#include "HealthSystemServer.h"

#include "BotComponent.h"
#include "ClientProxy.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
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

    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    Entity entityA = INVALID_ENTITY;
    Entity entityB = INVALID_ENTITY;

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    for (auto& entity : m_entities) {
        PlayerID playerID = serverComponent.GetPlayerID(entity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        if (entityA == INVALID_ENTITY) {
            entityA = entity;
        } else if (entityB == INVALID_ENTITY) {
            entityB = entity;
        }

        std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().GetComponent<SpriteComponent>(entity);
        std::weak_ptr<BotComponent> botComponent = g_gameServer->GetComponentManager().GetComponent<BotComponent>(entity);

        if (botComponent.lock()->m_actionType != BotComponent::ActionType::NONE) {
            continue;
        }

        U64 characterDirtyState = 0;

        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        for (U32 i = clientProxy.lock()->m_inputBuffer.m_front; i < clientProxy.lock()->m_inputBuffer.m_back; ++i) {
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

    EventComponent& eventComponent = g_game->GetEventComponent();
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_LSHIFT) == EventComponent::KeyState::REPEAT
        && eventComponent.m_keyboard.at(SDL_SCANCODE_W) == EventComponent::KeyState::DOWN) {
        OnCollisionEnter(entityA, entityB);
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

    healthComponentA.lock()->m_currentHP = 0;
    healthComponentB.lock()->m_currentHP = 0;

    Event newHealthEvent;
    newHealthEvent.eventType = EventType::HEALTH_EMPTIED;
    newHealthEvent.health.entity = entityA;
    NotifyEvent(newHealthEvent);
    newHealthEvent.health.entity = entityB;
    NotifyEvent(newHealthEvent);

    Event newComponentEvent;
    newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
    newComponentEvent.component.dirtyState = static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HP);
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

    healthComponent.lock()->m_currentHP -= damage;
    if (healthComponent.lock()->m_currentHP <= 0) {
        healthComponent.lock()->m_currentHP = 0;

        Event newHealthEvent;
        newHealthEvent.eventType = EventType::HEALTH_EMPTIED;
        newHealthEvent.health.entity = entity;
        NotifyEvent(newHealthEvent);
    }

    Event newComponentEvent;
    newComponentEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
    newComponentEvent.component.dirtyState = static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HP);
    newComponentEvent.component.entity = entity;
    NotifyEvent(newComponentEvent);
}
}
