#include "ClientReplicationManager.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "Game.h"
#include "LinkingContext.h"
#include "MemoryStream.h"
#include "ReplicationCommand.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ClientReplicationManager::ClientReplicationManager()
{
}

//----------------------------------------------------------------------------------------------------
ClientReplicationManager::~ClientReplicationManager()
{
}

//----------------------------------------------------------------------------------------------------
void ClientReplicationManager::Read(InputMemoryStream& inputStream) const
{
    while (inputStream.GetRemainingBitCount() >= 32) {
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

            ReadUpdateEntityAction(inputStream, networkID);

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
void ClientReplicationManager::ReadCreateEntityAction(InputMemoryStream& inputStream, NetworkID networkID) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);
    if (entity != INVALID_ENTITY) {
        ReadUpdateEntityAction(inputStream, networkID);
        return;
    }

    entity = g_game->GetEntityManager().AddEntity();
    g_game->GetLinkingContext().AddEntity(entity, networkID);

    Signature signature = 0;
    inputStream.Read(signature /*, GetRequiredBits<static_cast<U16>(MAX_COMPONENTS)>::value*/); // TODO: read server MAX_COMPONENTS

    U16 hasTransform = 1 << static_cast<std::size_t>(ComponentType::TRANSFORM);
    if (signature & hasTransform) {
        U16 memberFlags = 0;
        inputStream.Read(memberFlags, GetRequiredBits<static_cast<U16>(TransformComponent::MemberType::COUNT)>::value);
        std::shared_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().AddComponent<TransformComponent>(entity);
        transformComponent->Read(inputStream, memberFlags);
    }

    // TODO: read total size of things written
}

//----------------------------------------------------------------------------------------------------
void ClientReplicationManager::ReadUpdateEntityAction(InputMemoryStream& inputStream, NetworkID networkID) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);
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
        U16 memberFlags = 0;
        inputStream.Read(memberFlags, GetRequiredBits<static_cast<U16>(TransformComponent::MemberType::COUNT)>::value);
        std::shared_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        transformComponent->Read(inputStream, memberFlags);
    }

    // TODO: read total size of things written
}

//----------------------------------------------------------------------------------------------------
void ClientReplicationManager::ReadRemoveEntityAction(InputMemoryStream& /*inputStream*/, NetworkID networkID) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);
    if (entity == INVALID_ENTITY) {
        // TODO: read total size of things written
        //U32 bitCount = replicationHeader.GetBitCount();
        //WLOG("The entity has not been created yet. Advancing the memory stream's head %u bits...", bitCount);
        //inputStream.AdvanceHead(bitCount);
        return;
    }

    g_game->GetLinkingContext().RemoveEntity(entity);
    g_game->GetEntityManager().RemoveEntity(entity);
}
}
