#include "ReplicationManagerServer.h"

#include "ComponentManager.h"
#include "ComponentMemberTypes.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "LinkingContext.h"
#include "NetworkableWriteObject.h"
#include "ReplicationCommand.h"
#include "ReplicationResultManager.h"
#include "ServerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ReplicationManagerServer::ReplicationManagerServer()
    : m_networkIDToReplicationCommand()
{
}

//----------------------------------------------------------------------------------------------------
bool ReplicationManagerServer::AddCommand(NetworkID networkID, U64 dirtyState, bool isReplicated)
{
    auto it = m_networkIDToReplicationCommand.find(networkID);
    if (it != m_networkIDToReplicationCommand.end()) {
        WLOG("NetworkID %u is already registered", networkID);
        return false;
    }

    m_networkIDToReplicationCommand[networkID] = ReplicationCommand(ReplicationActionType::CREATE, dirtyState, isReplicated, false);

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
void ReplicationManagerServer::SetCreate(NetworkID networkID, U64 dirtyState)
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
void ReplicationManagerServer::SetIsReplicated(NetworkID networkID, bool isReplicated)
{
    m_networkIDToReplicationCommand.at(networkID).SetIsReplicated(isReplicated);
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerServer::AddDirtyState(NetworkID networkID, U64 dirtyState)
{
    m_networkIDToReplicationCommand.at(networkID).AddDirtyState(dirtyState);
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerServer::Remove(NetworkID networkID)
{
    ReplicationCommand& replicationCommand = m_networkIDToReplicationCommand.at(networkID);
    const bool hasReplicated = replicationCommand.GetHasReplicated();
    if (hasReplicated) {
        SetRemove(networkID);
    } else {
        RemoveCommand(networkID);
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerServer::Write(OutputMemoryStream& outputStream, ReplicationResultManager& replicationResultManager)
{
    U32 frame = MyTime::GetInstance().GetFrame();
    ILOG("Frame sent %u", frame);
    outputStream.Write(frame);

    for (auto& pair : m_networkIDToReplicationCommand) {
        ReplicationCommand& replicationCommand = pair.second;
        const bool isReplicated = replicationCommand.GetIsReplicated();
        const bool wasReplicated = replicationCommand.GetWasReplicated();
        const bool hasDirtyState = replicationCommand.HasDirtyState();
        const bool hasReplicationAction = replicationCommand.m_replicationActionType != ReplicationActionType::NONE;
        if ((!isReplicated && wasReplicated) || (isReplicated && !wasReplicated) || (isReplicated && hasDirtyState && hasReplicationAction)) {
            NetworkID networkID = pair.first;
            ILOG("Write networkID %u", networkID);
            outputStream.Write(networkID);
            outputStream.Write(isReplicated);
            outputStream.Write(wasReplicated);

            if (isReplicated) {
                outputStream.Write(replicationCommand.m_replicationActionType, 2);

                U64 writtenState = 0;

                switch (replicationCommand.m_replicationActionType) {

                case ReplicationActionType::CREATE: {
                    ILOG("CREATE SENT");
                    writtenState = WriteCreateAction(outputStream, networkID, replicationCommand.GetDirtyState());
                    break;
                }

                case ReplicationActionType::UPDATE: {
                    ILOG("UPDATE SENT");
                    writtenState = WriteUpdateAction(outputStream, networkID, replicationCommand.GetDirtyState());
                    break;
                }

                case ReplicationActionType::REMOVE: {
                    ILOG("REMOVE SENT");
                    break;
                }

                default: {
                    WLOG("Unknown replication action received from networkID %u", networkID);
                    break;
                }
                }
            }

            replicationResultManager.AddDelivery(networkID, replicationCommand);

            if (isReplicated) {
                replicationCommand.RemoveDirtyState(static_cast<U64>(ComponentMemberType::ALL));
                //replicationCommand.RemoveDirtyState(writtenState);
                replicationCommand.SetHasReplicated(true);

                if (replicationCommand.m_replicationActionType == ReplicationActionType::CREATE
                    || replicationCommand.m_replicationActionType == ReplicationActionType::REMOVE) {
                    replicationCommand.m_replicationActionType = ReplicationActionType::NONE;
                }
            }

            if (!isReplicated && wasReplicated) {
                replicationCommand.SetWasReplicated(false);
            } else if (isReplicated && !wasReplicated) {
                replicationCommand.SetWasReplicated(true);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
U64 ReplicationManagerServer::WriteCreateAction(OutputMemoryStream& outputStream, NetworkID networkID, U64 dirtyState) const
{
    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    Entity entity = g_gameServer->GetLinkingContext().GetEntity(networkID);
    PlayerID playerID = serverComponent.lock()->GetPlayerID(entity);
    outputStream.Write(playerID);

    return WriteUpdateAction(outputStream, networkID, dirtyState);
}

//----------------------------------------------------------------------------------------------------
U64 ReplicationManagerServer::WriteUpdateAction(OutputMemoryStream& outputStream, NetworkID networkID, U64 dirtyState) const
{
    U64 writtenState = 0;

    Entity entity = g_gameServer->GetLinkingContext().GetEntity(networkID);
    Signature signature = g_gameServer->GetEntityManager().GetSignature(entity);
    outputStream.Write(signature);
    outputStream.Write(dirtyState);

    for (U16 i = 0; i < MAX_NETWORK_COMPONENTS; ++i) {

        U16 hasComponent = 1 << i;
        const bool hasSignatureComponent = signature & hasComponent;
        if (hasSignatureComponent) {
            std::weak_ptr<Component> component = g_gameServer->GetComponentManager().GetComponent(static_cast<ComponentType>(i), entity);
            writtenState |= std::dynamic_pointer_cast<NetworkableWriteObject>(component.lock())->Write(outputStream, dirtyState);
        }
    }

    return writtenState;
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerServer::Reset()
{
    m_networkIDToReplicationCommand.clear();
}
}
