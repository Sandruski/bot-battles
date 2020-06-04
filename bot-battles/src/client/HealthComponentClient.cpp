#include "HealthComponent.h"

#include "ClientComponent.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "ReplicationManagerClient.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void HealthComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType replicationActionType, Entity entity)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HP)) {
        I32 oldCurrentHP = m_currentHP;
        inputStream.Read(m_currentHP);

        if (replicationActionType != ReplicationActionType::CREATE) {
            Event newHealthEvent;
            if (oldCurrentHP > m_currentHP) {
                newHealthEvent.eventType = EventType::HEALTH_LOST;
                newHealthEvent.health.health = oldCurrentHP - m_currentHP;
            } else if (oldCurrentHP < m_currentHP) {
                newHealthEvent.eventType = EventType::HEALTH_GAINED;
                newHealthEvent.health.health = m_currentHP - oldCurrentHP;
            }
            newHealthEvent.health.entity = entity;
            std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
            clientComponent.lock()->m_replicationManager.NotifyEvent(newHealthEvent);
        }
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_MAX_HP)) {
        inputStream.Read(m_maxHP);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_HP)) {
        inputStream.Read(m_HP);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_TIME_HEAL)) {
        inputStream.Read(m_timeHeal);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_COOLDOWN_HEAL)) {
        inputStream.Read(m_cooldownHeal);
    }
}
}
