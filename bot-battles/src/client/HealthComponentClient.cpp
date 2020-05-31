#include "HealthComponent.h"

#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "ReplicationManagerClient.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void HealthComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType replicationActionType, Entity entity)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_CURRENT_HEALTH)) {
        I32 oldCurrentHealth = m_currentHealth;
        inputStream.Read(m_currentHealth);

        if (replicationActionType != ReplicationActionType::CREATE) {
            Event newHealthEvent;
            if (oldCurrentHealth > m_currentHealth) {
                newHealthEvent.eventType = EventType::HEALTH_LOST;
                newHealthEvent.health.health = oldCurrentHealth - m_currentHealth;
            } else if (oldCurrentHealth < m_currentHealth) {
                newHealthEvent.eventType = EventType::HEALTH_GAINED;
                newHealthEvent.health.health = m_currentHealth - oldCurrentHealth;
            }
            newHealthEvent.health.entity = entity;
            ClientComponent& clientComponent = g_gameClient->GetClientComponent();
            clientComponent.m_replicationManager.NotifyEvent(newHealthEvent);
        }
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::HEALTH_MAX_HEALTH)) {
        inputStream.Read(m_maxHealth);
    }
}
}
