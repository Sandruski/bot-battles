#include "ReplicationManagerClient.h"

#include "ClientComponent.h"
#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "EntityManager.h"
#include "GameClient.h"
#include "LinkingContext.h"
#include "NetworkableReadObject.h"
#include "ReplicationCommand.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::Read(InputMemoryStream& inputStream) const
{
    while (inputStream.GetRemainingBitCount() >= 8) {

        NetworkID networkID = INVALID_NETWORK_ID;
        inputStream.Read(networkID);

        ReplicationActionType replicationActionType = ReplicationActionType::NONE;
        inputStream.Read(replicationActionType, 2);

        switch (replicationActionType) {

        case ReplicationActionType::CREATE: {
            ReadCreateAction(inputStream, networkID);
            break;
        }

        case ReplicationActionType::UPDATE: {
            ReadUpdateAction(inputStream, networkID);
            break;
        }

        case ReplicationActionType::REMOVE: {
            ReadRemoveAction(networkID);
            break;
        }

        default: {
            WLOG("Unknown replication action received from networkID %u", networkID);
            break;
        }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::ReadCreateAction(InputMemoryStream& inputStream, NetworkID networkID) const
{
    Entity entity = g_gameClient->GetLinkingContext().GetEntity(networkID);
    if (entity == INVALID_ENTITY) {
        entity = g_gameClient->GetEntityManager().AddEntity();
        g_gameClient->GetLinkingContext().AddEntity(entity, networkID);
    }

    PlayerID playerID = INVALID_PLAYER_ID;
    inputStream.Read(playerID);
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    if (playerID == clientComponent.m_playerID) {
        clientComponent.m_entity = entity;
    }

    ReadUpdateAction(inputStream, networkID);
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::ReadUpdateAction(InputMemoryStream& inputStream, NetworkID networkID) const
{
    Entity entity = g_gameClient->GetLinkingContext().GetEntity(networkID);
    Signature signature = g_gameClient->GetEntityManager().GetSignature(entity);

    Signature newSignature = 0;
    inputStream.Read(newSignature);
    U32 dirtyState = 0;
    inputStream.Read(dirtyState, GetRequiredBits<static_cast<U32>(ComponentMemberType::COUNT)>::value);

    for (U16 i = 0; i < MAX_COMPONENTS; ++i) {

        U16 hasComponent = 1 << i;
        const bool hasSignatureComponent = signature & hasComponent;
        const bool hasNewSignatureComponent = newSignature & hasComponent;
        if (hasSignatureComponent && hasNewSignatureComponent) {
            std::weak_ptr<Component> component = g_gameClient->GetComponentManager().GetComponent(static_cast<ComponentType>(i), entity);
            std::dynamic_pointer_cast<NetworkableReadObject>(component.lock())->Read(inputStream, dirtyState, ReplicationActionType::UPDATE, entity);
        } else if (hasSignatureComponent) {
            g_gameClient->GetComponentManager().RemoveComponent(static_cast<ComponentType>(i), entity);
        } else if (hasNewSignatureComponent) {
            std::weak_ptr<Component> component = g_gameClient->GetComponentManager().AddComponent(static_cast<ComponentType>(i), entity);
            std::dynamic_pointer_cast<NetworkableReadObject>(component.lock())->Read(inputStream, dirtyState, ReplicationActionType::CREATE, entity);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::ReadRemoveAction(NetworkID networkID) const
{
    Entity entity = g_gameClient->GetLinkingContext().GetEntity(networkID);
    if (entity == INVALID_ENTITY) {
        return;
    }

    g_gameClient->GetLinkingContext().RemoveEntity(entity);
    g_gameClient->GetEntityManager().RemoveEntity(entity);
}
}
