#include "ReplicationManagerServer.h"

#include "ComponentManager.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "InputComponent.h"
#include "LinkingContext.h"
#include "MemoryStream.h"
#include "ReplicationCommand.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "TextComponent.h"

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
bool ReplicationManagerServer::CreateCommand(NetworkID networkID, U32 dirtyState)
{
    auto it = m_networkIDToReplicationCommand.find(networkID);
    if (it != m_networkIDToReplicationCommand.end()) {
        WLOG("NetworkID %u is already registered", networkID);
        return false;
    }

    m_networkIDToReplicationCommand[networkID] = ReplicationCommand(dirtyState);

    return true;
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerServer::RemoveCommand(NetworkID networkID)
{
    m_networkIDToReplicationCommand.at(networkID).m_replicationActionType = ReplicationActionType::REMOVE;
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerServer::AddDirtyState(NetworkID networkID, U32 dirtyState)
{
    m_networkIDToReplicationCommand.at(networkID).AddDirtyState(dirtyState);
}

//----------------------------------------------------------------------------------------------------
void ReplicationManagerServer::Write(OutputMemoryStream& outputStream)
{
    for (auto& pair : m_networkIDToReplicationCommand) {

        ReplicationCommand& replicationCommand = pair.second;
        if (replicationCommand.HasDirtyState()) {
            NetworkID networkID = pair.first;
            outputStream.Write(networkID);

            ReplicationActionType replicationActionType = replicationCommand.m_replicationActionType;
            outputStream.Write(replicationActionType, 2);

            U32 writtenState = 0;

            switch (replicationActionType) {

            case ReplicationActionType::CREATE: {
                writtenState = WriteCreateOrUpdateAction(outputStream, networkID, replicationCommand.GetDirtyState());
                replicationActionType = ReplicationActionType::UPDATE;
                break;
            }

            case ReplicationActionType::UPDATE: {
                writtenState = WriteCreateOrUpdateAction(outputStream, networkID, replicationCommand.GetDirtyState());
                break;
            }

            case ReplicationActionType::REMOVE: {
                replicationActionType = ReplicationActionType::UPDATE;
                break;
            }

            default: {
                WLOG("Unknown replication action received from networkID %u", networkID);
                break;
            }
            }

            replicationCommand.RemoveDirtyState(writtenState);
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

//----------------------------------------------------------------------------------------------------
void ReplicationManagerServer::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::DELIVERY_SUCCESS: {

        break;
    }

    case EventType::DELIVERY_FAILURE: {

        break;
    }

    default: {
        break;
    }
    }
}
}
