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

    (*it).second.SetRemoveEntityAction();

    return true;
}

//----------------------------------------------------------------------------------------------------
void ServerReplicationManager::ProcessCommands(OutputMemoryStream& outputStream)
{
    for (auto& pair : m_networkIDToReplicationCommand) {

        ReplicationCommand& replicationCommand = pair.second;
        if (replicationCommand.IsDirty()) {
            NetworkID networkID = pair.first;
            outputStream.Write(networkID);

            ReplicationAction replicationAction = replicationCommand.GetAction();
            outputStream.Write(replicationAction, GetRequiredBits<static_cast<U8>(ReplicationAction::COUNT)>::value);

            /*
			uint32_t writtenState = 0;
			uint32_t dirtyState = replicationCommand.GetDirtyState();
			*/

            switch (replicationAction) {

            case ReplicationAction::CREATE_ENTITY: {
                break;
            }

            case ReplicationAction::UPDATE_ENTITY: {
                break;
            }

            case ReplicationAction::REMOVE_ENTITY: {
                break;
            }

            default: {
                break;
            }
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerReplicationManager::WriteCreateEntityAction(OutputMemoryStream& outputStream, NetworkID networkID) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);
    Signature signature = g_game->GetEntityManager().GetSignature(entity);
    outputStream.Write(signature.to_ulong(), signature.size());

    if (signature.test(static_cast<std::size_t>(ComponentType::TRANSFORM))) {

        std::shared_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        transformComponent->Write(outputStream, static_cast<U16>(TransformComponent::MemberType::ALL));
    }

    // TODO: Write total size of things written
}

//----------------------------------------------------------------------------------------------------
void ServerReplicationManager::WriteUpdateEntityAction(OutputMemoryStream& outputStream, NetworkID networkID) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);
    Signature signature = g_game->GetEntityManager().GetSignature(entity);
    outputStream.Write(signature.to_ulong(), signature.size());

    if (signature.test(static_cast<std::size_t>(ComponentType::TRANSFORM))) {

        std::shared_ptr<TransformComponent> transformComponent = g_game->GetComponentManager().GetComponent<TransformComponent>(entity);
        transformComponent->Write(outputStream, static_cast<U16>(TransformComponent::MemberType::ALL));
    }
}
}
