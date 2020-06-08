#include "HealthComponent.h"

#include "ClientComponent.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "ReplicationManagerClient.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void HealthComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType replicationActionType, Entity entity)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_HIT_ENTITY_LAST_SHOT)) {
        inputStream.Read(m_hitEntityLastShot);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HP)) {
        I32 oldCurrentHP = m_currentHP;
        inputStream.Read(m_currentHP);

        if (replicationActionType != ReplicationActionType::CREATE) {
            Event newHealthEvent;
            if (oldCurrentHP > m_currentHP) {
                newHealthEvent.eventType = EventType::HEALTH_HURT;
                newHealthEvent.health.health = oldCurrentHP - m_currentHP;
                newHealthEvent.health.shooterEntity = m_hitEntityLastShot;
            } else if (oldCurrentHP < m_currentHP) {
                newHealthEvent.eventType = EventType::HEALTH_HEALED;
                newHealthEvent.health.health = m_currentHP - oldCurrentHP;
            }
            newHealthEvent.health.targetEntity = entity;
            std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
            clientComponent.lock()->m_replicationManager.NotifyEvent(newHealthEvent);
        }
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_MAX_HP)) {
        inputStream.Read(m_maxHP);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_HP)) {
        inputStream.Read(m_HP);

        if (replicationActionType != ReplicationActionType::CREATE) {
            if (m_HP > 0) {
                Event newHealthEvent;
                newHealthEvent.eventType = EventType::HEALTH_PICKED_UP;
                newHealthEvent.health.health = m_HP;
                newHealthEvent.health.targetEntity = entity;
                std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
                clientComponent.lock()->m_replicationManager.NotifyEvent(newHealthEvent);
            }
        }
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_TIME_HEAL)) {
        inputStream.Read(m_timeHeal);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_COOLDOWN_HEAL)) {
        inputStream.Read(m_cooldownHeal);
    }
}
}
