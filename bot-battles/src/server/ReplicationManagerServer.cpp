#include "ReplicationManagerServer.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "MemoryStream.h"
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
ReplicationManagerServer::~ReplicationManagerServer()
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
            case ReplicationActionType::CREATE:
            case ReplicationActionType::UPDATE: {
                writtenState = WriteCreateOrUpdateAction(outputStream, networkID, replicationCommand.GetDirtyState());
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
U32 ReplicationManagerServer::WriteCreateOrUpdateAction(OutputMemoryStream& outputStream, NetworkID networkID, U32 dirtyState) const
{
    U32 writtenState = 0;

    Entity entity = g_gameServer->GetLinkingContext().GetEntity(networkID);
    Signature signature = g_gameServer->GetEntityManager().GetSignature(entity);
    outputStream.Write(signature);
    outputStream.Write(dirtyState, GetRequiredBits<static_cast<U32>(ComponentMemberType::COUNT)>::value);

    U16 hasInput = 1 << static_cast<std::size_t>(ComponentType::INPUT);
    const bool hasSignatureInput = signature & hasInput;
    if (hasSignatureInput) {
        std::shared_ptr<InputComponent> inputComponent = g_gameServer->GetComponentManager().GetComponent<InputComponent>(entity);
        writtenState |= inputComponent->Write(outputStream, dirtyState);
    }

    U16 hasTransform = 1 << static_cast<std::size_t>(ComponentType::TRANSFORM);
    const bool hasSignatureTransform = signature & hasTransform;
    if (hasSignatureTransform) {
        std::shared_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().GetComponent<TransformComponent>(entity);
        writtenState |= transformComponent->Write(outputStream, dirtyState);
    }

    U16 hasSprite = 1 << static_cast<std::size_t>(ComponentType::SPRITE);
    const bool hasSignatureSprite = signature & hasSprite;
    if (hasSignatureSprite) {
        std::shared_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().GetComponent<SpriteComponent>(entity);
        writtenState |= spriteComponent->Write(outputStream, dirtyState);
    }

    U16 hasText = 1 << static_cast<std::size_t>(ComponentType::TEXT);
    const bool hasSignatureText = signature & hasText;
    if (hasSignatureText) {
        std::shared_ptr<TextComponent> textComponent = g_gameServer->GetComponentManager().GetComponent<TextComponent>(entity);
        writtenState |= textComponent->Write(outputStream, dirtyState);
    }

    // TODO: write total size of things written

    return writtenState;
}
}
