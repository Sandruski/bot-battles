#include "ReplicationManagerClient.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameClient.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "MemoryStream.h"
#include "ReplicationCommand.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ReplicationManagerClient::ReplicationManagerClient()
{
}

//----------------------------------------------------------------------------------------------------
ReplicationManagerClient::~ReplicationManagerClient()
{
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::Read(InputMemoryStream& inputStream) const
{
    while (inputStream.GetRemainingBitCount() >= 8) {

        NetworkID networkID = INVALID_NETWORK_ID;
        inputStream.Read(networkID);

        ReplicationAction replicationAction = ReplicationAction::INVALID;
        inputStream.Read(replicationAction, GetRequiredBits<static_cast<U8>(ReplicationAction::COUNT)>::value);

        switch (replicationAction) {

        case ReplicationAction::CREATE_ENTITY: {

            ReadCreateEntityAction(inputStream, networkID);

            break;
        }

        case ReplicationAction::UPDATE_ENTITY: {

            ReadUpdateEntityAction(inputStream, networkID);

            break;
        }

        case ReplicationAction::REMOVE_ENTITY: {

            ReadRemoveEntityAction(inputStream, networkID);

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
void ReplicationManagerClient::ReadCreateEntityAction(InputMemoryStream& inputStream, NetworkID networkID) const
{
    Entity entity = g_gameClient->GetLinkingContext().GetEntity(networkID);
    if (entity != INVALID_ENTITY) {
        ReadUpdateEntityAction(inputStream, networkID);
        return;
    }

    entity = g_gameClient->GetEntityManager().AddEntity();
    g_gameClient->GetLinkingContext().AddEntity(entity, networkID);

    Signature signature = 0;
    inputStream.Read(signature /*, GetRequiredBits<static_cast<U16>(MAX_COMPONENTS)>::value*/); // TODO: read server MAX_COMPONENTS

    U16 hasTransform = 1 << static_cast<std::size_t>(ComponentType::TRANSFORM);
    if (signature & hasTransform) {
        std::shared_ptr<TransformComponent> transform = g_gameClient->GetComponentManager().AddComponent<TransformComponent>(entity);
        transform->Read(inputStream);
    }
    U16 hasInput = 1 << static_cast<std::size_t>(ComponentType::INPUT);
    if (signature & hasInput) {
        g_gameClient->GetComponentManager().AddComponent<InputComponent>(entity);
    }

    // TODO: read total size of things written
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::ReadUpdateEntityAction(InputMemoryStream& inputStream, NetworkID networkID) const
{
    Entity entity = g_gameClient->GetLinkingContext().GetEntity(networkID);
    if (entity == INVALID_ENTITY) {
        //U32 bitCount = replicationHeader.GetBitCount();
        //WLOG("The entity has not been created yet. Advancing the memory stream's head %u bits...", bitCount);
        //inputStream.AdvanceHead(bitCount);
        return;
    }

    Signature signature = 0;
    inputStream.Read(signature /*, GetRequiredBits<static_cast<U16>(MAX_COMPONENTS)>::value*/); // TODO: read server MAX_COMPONENTS

    U16 hasTransform = 1 << static_cast<std::size_t>(ComponentType::TRANSFORM);
    if (signature & hasTransform) {
        std::shared_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().GetComponent<TransformComponent>(entity);
        transformComponent->Read(inputStream);
    }

    // TODO: read total size of things written
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerClient::ReadRemoveEntityAction(InputMemoryStream& /*inputStream*/, NetworkID networkID) const
{
    Entity entity = g_gameClient->GetLinkingContext().GetEntity(networkID);
    if (entity == INVALID_ENTITY) {
        // TODO: read total size of things written
        //U32 bitCount = replicationHeader.GetBitCount();
        //WLOG("The entity has not been created yet. Advancing the memory stream's head %u bits...", bitCount);
        //inputStream.AdvanceHead(bitCount);
        return;
    }

    g_gameClient->GetLinkingContext().RemoveEntity(entity);
    g_gameClient->GetEntityManager().RemoveEntity(entity);
}
}
