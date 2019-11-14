#include "ServerReplicationManager.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "Game.h"
#include "LinkingContext.h"
#include "MemoryStream.h"
#include "ReplicationCommand.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ServerReplicationManager::ServerReplicationManager()
    : m_networkIDToReplicationCommand()
{
}

//----------------------------------------------------------------------------------------------------
ServerReplicationManager::~ServerReplicationManager()
{
}

//----------------------------------------------------------------------------------------------------
bool ServerReplicationManager::CreateEntityCommand(NetworkID networkID)
{
    auto it = m_networkIDToReplicationCommand.find(networkID);
    if (it != m_networkIDToReplicationCommand.end()) {
        WLOG("NetworkID %u is already registered", networkID);
        return false;
    }

    m_networkIDToReplicationCommand[networkID] = ReplicationCommand();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ServerReplicationManager::RemoveEntityCommand(NetworkID networkID)
{
    auto it = m_networkIDToReplicationCommand.find(networkID);
    if (it == m_networkIDToReplicationCommand.end()) {
        WLOG("NetworkID %u is not registered", networkID);
        return false;
    }

    (*it).second.m_replicationAction = ReplicationAction::REMOVE_ENTITY;

    return true;
}

//----------------------------------------------------------------------------------------------------
void ServerReplicationManager::Write(OutputMemoryStream& outputStream)
{
    for (auto& pair : m_networkIDToReplicationCommand) {

        ReplicationCommand& replicationCommand = pair.second;
        //if (replicationCommand.IsDirty()) {

        NetworkID networkID = pair.first;
        outputStream.Write(networkID);

        ReplicationAction replicationAction = replicationCommand.m_replicationAction;
        outputStream.Write(replicationAction, GetRequiredBits<static_cast<U8>(ReplicationAction::COUNT)>::value);

        /*
			uint32_t writtenState = 0;
			uint32_t dirtyState = replicationCommand.GetDirtyState();
			*/

        switch (replicationAction) {

        case ReplicationAction::CREATE_ENTITY: {

            WriteCreateEntityAction(outputStream, networkID);
            replicationCommand.m_replicationAction = ReplicationAction::UPDATE_ENTITY;

            break;
        }

        case ReplicationAction::UPDATE_ENTITY: {

            WriteUpdateEntityAction(outputStream, networkID);

            break;
        }

        case ReplicationAction::REMOVE_ENTITY: {

            // Nothing to do here...

            break;
        }

        default: {
            WLOG("Unknown replication action received from networkID %u", networkID);
            break;
        }
        }
        //}
    }

    // TODO
}

//----------------------------------------------------------------------------------------------------
void ServerReplicationManager::WriteCreateEntityAction(OutputMemoryStream& outputStream, NetworkID networkID) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);

    Signature signature = g_game->GetEntityManager().GetSignature(entity); // Signature contains ALL components
    outputStream.Write(signature /*, GetRequiredBits<static_cast<U16>(MAX_COMPONENTS)>::value*/); // TODO: write server MAX_COMPONENTS

    U16 hasTransform = 1 << static_cast<std::size_t>(ComponentType::TRANSFORM);
    if (signature & hasTransform) {
        U16 memberFlags = static_cast<U16>(TransformComponent::MemberType::ALL); // MemberFlags contains ALL members
        std::shared_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        transformComponent->Write(outputStream, memberFlags);
    }

    // TODO: write total size of things written
}

//----------------------------------------------------------------------------------------------------
void ServerReplicationManager::WriteUpdateEntityAction(OutputMemoryStream& outputStream, NetworkID networkID) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);

    Signature signature = g_game->GetEntityManager().GetSignature(entity); // Signature contains ONLY changed components
    outputStream.Write(signature /*, GetRequiredBits<static_cast<U16>(MAX_COMPONENTS)>::value*/); // TODO: write max server components

    U16 hasTransform = 1 << static_cast<std::size_t>(ComponentType::TRANSFORM);
    if (signature & hasTransform) {
        U16 memberFlags = static_cast<U16>(TransformComponent::MemberType::ALL); // MemberFlags contains ONLY changed memebers
        std::shared_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        transformComponent->Write(outputStream, memberFlags);
    }

    // TODO: write total size of things written
}
}
