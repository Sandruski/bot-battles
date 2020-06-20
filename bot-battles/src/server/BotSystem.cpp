#include "BotSystem.h"

#include "BotComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "GameServer.h"
#include "GameplayComponent.h"
#include "SpriteComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
BotSystem::BotSystem()
{
    m_signature |= 1 << static_cast<U16>(ComponentType::BOT);
    m_signature |= 1 << static_cast<U16>(ComponentType::PLAYER);
}

//----------------------------------------------------------------------------------------------------
bool BotSystem::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool BotSystem::Update()
{
    OPTICK_EVENT();

    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameServer->GetGameplayComponent();
    std::weak_ptr<State> currentState = gameplayComponent.lock()->m_fsm.GetCurrentState();
    if (currentState.expired()) {
        return true;
    }

    for (auto& entity : m_entities) {
        std::weak_ptr<BotComponent> botComponent = g_gameServer->GetComponentManager().GetComponent<BotComponent>(entity);
        std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().GetComponent<SpriteComponent>(entity);

        U64 characterDirtyState = 0;

        switch (botComponent.lock()->m_actionType) {
        case BotComponent::ActionType::SHOOT_PRIMARY: {
            if (botComponent.lock()->m_timerAction.ReadSec() >= botComponent.lock()->m_timeAction) {
                spriteComponent.lock()->m_spriteName = "idle";
                characterDirtyState |= static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME);

                botComponent.lock()->m_actionType = BotComponent::ActionType::COOLDOWN_SHOOT_PRIMARY;
                characterDirtyState |= static_cast<U64>(ComponentMemberType::BOT_ACTION_TYPE);
            }
            break;
        }

        case BotComponent::ActionType::SHOOT_SECONDARY: {
            if (botComponent.lock()->m_timerAction.ReadSec() >= botComponent.lock()->m_timeAction) {
                spriteComponent.lock()->m_spriteName = "idle";
                characterDirtyState |= static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME);

                botComponent.lock()->m_actionType = BotComponent::ActionType::COOLDOWN_SHOOT_SECONDARY;
                characterDirtyState |= static_cast<U64>(ComponentMemberType::BOT_ACTION_TYPE);
            }
            break;
        }

        case BotComponent::ActionType::RELOAD: {
            if (botComponent.lock()->m_timerAction.ReadSec() >= botComponent.lock()->m_timeAction) {
                spriteComponent.lock()->m_spriteName = "idle";
                characterDirtyState |= static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME);

                botComponent.lock()->m_actionType = BotComponent::ActionType::COOLDOWN_RELOAD;
                characterDirtyState |= static_cast<U64>(ComponentMemberType::BOT_ACTION_TYPE);
            }
            break;
        }

        case BotComponent::ActionType::HEAL: {
            if (botComponent.lock()->m_timerAction.ReadSec() >= botComponent.lock()->m_timeAction) {
                spriteComponent.lock()->m_spriteName = "idle";
                characterDirtyState |= static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME);

                botComponent.lock()->m_actionType = BotComponent::ActionType::COOLDOWN_HEAL;
                characterDirtyState |= static_cast<U64>(ComponentMemberType::BOT_ACTION_TYPE);
            }
            break;
        }

        case BotComponent::ActionType::COOLDOWN_SHOOT_PRIMARY:
        case BotComponent::ActionType::COOLDOWN_SHOOT_SECONDARY:
        case BotComponent::ActionType::COOLDOWN_RELOAD:
        case BotComponent::ActionType::COOLDOWN_HEAL: {
            if (botComponent.lock()->m_timerAction.ReadSec() >= botComponent.lock()->m_timeAction + botComponent.lock()->m_cooldownAction) {
                botComponent.lock()->m_actionType = BotComponent::ActionType::NONE;
                characterDirtyState |= static_cast<U64>(ComponentMemberType::BOT_ACTION_TYPE);
            }
            break;
        }

        case BotComponent::ActionType::DIE: {
            if (botComponent.lock()->m_timerAction.ReadSec() >= botComponent.lock()->m_timeAction) {
                Event newHealthEvent;
                newHealthEvent.eventType = EventType::HEALTH_HURT;
                newHealthEvent.health.targetEntity = entity;
                NotifyEvent(newHealthEvent);
            }
            break;
        }

        case BotComponent::ActionType::KILL:
        case BotComponent::ActionType::NONE:
        default: {
            break;
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
}
