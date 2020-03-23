#include "HealthSystem.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "HealthComponent.h"
#include "SpriteComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
HealthSystem::HealthSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::HEALTH);
    m_signature |= 1 << static_cast<U16>(ComponentType::SPRITE);
}

//----------------------------------------------------------------------------------------------------
bool HealthSystem::Update()
{
    GameplayComponent& gameplayComponent = g_gameServer->GetGameplayComponent();
    if (gameplayComponent.m_phase != GameplayComponent::GameplayPhase::PLAY) {
        return true;
    }

    for (auto& entity : m_entities) {
        std::weak_ptr<HealthComponent> healthComponent = g_gameServer->GetComponentManager().GetComponent<HealthComponent>(entity);
        if (healthComponent.lock()->m_health <= 0) {
            healthComponent.lock()->m_health = 0;

            std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().GetComponent<SpriteComponent>(entity);
            spriteComponent.lock()->m_isEnabled = false;

            Event newEvent;
            newEvent.eventType = EventType::COMPONENT_MEMBER_CHANGED;
            newEvent.component.entity = entity;
            newEvent.component.dirtyState = static_cast<U32>(ComponentMemberType::HEALTH_HEALTH) | static_cast<U32>(ComponentMemberType::SPRITE_ENABLED);
            NotifyEvent(newEvent);
        }
    }

    return true;
}
}
