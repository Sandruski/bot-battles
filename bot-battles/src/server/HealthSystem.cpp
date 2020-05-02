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
bool HealthSystem::PostUpdate()
{
    // TODO: do we really need this system with insta kill?
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    for (auto& entity : m_entities) {
        std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entity);
        if (!healthComponent.lock()->m_isDead && healthComponent.lock()->m_health <= 0) {
            healthComponent.lock()->m_isDead = true;
            healthComponent.lock()->m_health = 0;

            std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().GetComponent<SpriteComponent>(entity);
            spriteComponent.lock()->m_isEnabled = false;

            Event newEvent;
            newEvent.eventType = EventType::HEALTH_EMPTIED;
            newEvent.component.entity = entity;
            NotifyEvent(newEvent);

            newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
            newEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::HEALTH_HEALTH) | static_cast<U32>(ComponentMemberType::HEALTH_DEAD) | static_cast<U32>(ComponentMemberType::SPRITE_ENABLED);
            NotifyEvent(newEvent);
        }
    }

    return true;
}
}
