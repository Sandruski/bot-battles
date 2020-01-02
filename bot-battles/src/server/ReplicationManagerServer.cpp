#include "ReplicationManagerServer.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "ReplicationCommand.h"
#include "ReplicationResultManager.h"
#include "SpriteComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ReplicationManagerServer::ReplicationManagerServer()
    : m_networkIDToReplicationCommand()
{
}

//----------------------------------------------------------------------------------------------------
bool ReplicationManagerServer::AddCommand(NetworkID networkID, U32 dirtyState)
{
    auto it = m_networkIDToReplicationCommand.find(networkID);
    if (it != m_networkIDToReplicationCommand.end()) {
        WLOG("NetworkID %u is already registered", networkID);
        return false;
    }

    m_networkIDToReplicationCommand[networkID] = ReplicationCommand(ReplicationActionType::CREATE, dirtyState);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ReplicationManagerServer::RemoveCommand(NetworkID networkID)
{
    auto it = m_networkIDToReplicationCommand.find(networkID);
    if (it == m_networkIDToReplicationCommand.end()) {
        WLOG("NetworkID %u is not registered", networkID);
        return false;
    }

    m_networkIDToReplicationCommand.erase(it);

    return true;
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerServer::SetCreate(NetworkID networkID, U32 dirtyState)
{
    ReplicationCommand& replicationCommand = m_networkIDToReplicationCommand.at(networkID);
    if (replicationCommand.m_replicationActionType != ReplicationActionType::REMOVE) {
        replicationCommand.m_replicationActionType = ReplicationActionType::CREATE;
        replicationCommand.AddDirtyState(dirtyState);
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerServer::SetUpdate(NetworkID networkID)
{
    ReplicationCommand& replicationCommand = m_networkIDToReplicationCommand.at(networkID);
    if (replicationCommand.m_replicationActionType != ReplicationActionType::REMOVE) {
        replicationCommand.m_replicationActionType = ReplicationActionType::UPDATE;
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerServer::SetRemove(NetworkID networkID)
{
    m_networkIDToReplicationCommand.at(networkID).m_replicationActionType = ReplicationActionType::REMOVE;
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerServer::AddDirtyState(NetworkID networkID, U32 dirtyState)
{
    m_networkIDToReplicationCommand.at(networkID).AddDirtyState(dirtyState);
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerServer::Write(OutputMemoryStream& outputStream, ReplicationResultManager& replicationResultManager)
{
    for (auto& pair : m_networkIDToReplicationCommand) {

        ReplicationCommand& replicationCommand = pair.second;
        const bool hasDirtyState = replicationCommand.HasDirtyState();
        const bool hasReplicationAction = replicationCommand.m_replicationActionType != ReplicationActionType::NONE;

        if (hasDirtyState && hasReplicationAction) {

            NetworkID networkID = pair.first;
            outputStream.Write(networkID);
            outputStream.Write(replicationCommand.m_replicationActionType, 2);

            U32 writtenState = 0;

            switch (replicationCommand.m_replicationActionType) {

            case ReplicationActionType::CREATE: {
                writtenState = WriteCreateAction(outputStream, networkID, replicationCommand.GetDirtyState());
                break;
            }

            case ReplicationActionType::UPDATE: {
                writtenState = WriteUpdateAction(outputStream, networkID, replicationCommand.GetDirtyState());
                break;
            }

            default: {
                WLOG("Unknown replication action received from networkID %u", networkID);
                break;
            }
            }

            replicationResultManager.AddDelivery(networkID, replicationCommand);

            replicationCommand.RemoveDirtyState(writtenState);

            if (replicationCommand.m_replicationActionType == ReplicationActionType::CREATE
                || replicationCommand.m_replicationActionType == ReplicationActionType::REMOVE) {
                replicationCommand.m_replicationActionType = ReplicationActionType::NONE;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
U32 ReplicationManagerServer::WriteCreateAction(OutputMemoryStream& outputStream, NetworkID networkID, U32 dirtyState) const
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    Entity entity = g_gameServer->GetLinkingContext().GetEntity(networkID);
    PlayerID playerID = serverComponent.GetPlayerID(entity);
    outputStream.Write(playerID);

    return WriteUpdateAction(outputStream, networkID, dirtyState);
}

//----------------------------------------------------------------------------------------------------
U32 ReplicationManagerServer::WriteUpdateAction(OutputMemoryStream& outputStream, NetworkID networkID, U32 dirtyState) const
{
    U32 writtenState = 0;

    Entity entity = g_gameServer->GetLinkingContext().GetEntity(networkID);
    Signature signature = g_gameServer->GetEntityManager().GetSignature(entity);
    outputStream.Write(signature);
    outputStream.Write(dirtyState, GetRequiredBits<static_cast<U32>(ComponentMemberType::COUNT)>::value);

    for (U16 i = 0; i < MAX_COMPONENTS; ++i) {
        U16 hasComponent = 1 << i;
        const bool hasSignatureComponent = signature & hasComponent;
        if (hasSignatureComponent) {
            std::weak_ptr<Component> component = g_gameServer->GetComponentManager().GetBaseComponent(static_cast<ComponentType>(i), entity);
            writtenState |= component.lock()->Write(outputStream, dirtyState);
        }
    }

    return writtenState;
}
}
