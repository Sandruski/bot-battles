#include "BotComponent.h"

#include "ClientComponent.h"
#include "ComponentMemberTypes.h"
#include "GameClient.h"
#include "ReplicationManagerClient.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void BotComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity entity)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::BOT_ACTION_TYPE)) {
        inputStream.Read(m_actionType);

        switch (m_actionType) {
        case BotComponent::ActionType::KILL: {
            Event newEntityEvent;
            newEntityEvent.eventType = EventType::KILL;
            newEntityEvent.entity.entity = entity;
            std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
            clientComponent.lock()->m_replicationManager.NotifyEvent(newEntityEvent);
            break;
        }
        case BotComponent::ActionType::DIE: {
            Event newEntityEvent;
            newEntityEvent.eventType = EventType::DEATH;
            newEntityEvent.entity.entity = entity;
            std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
            clientComponent.lock()->m_replicationManager.NotifyEvent(newEntityEvent);
            break;
        }
        default: {
            break;
        }
        }
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::BOT_TIME_ACTION)) {
        inputStream.Read(m_timeAction);
    }
}
}
