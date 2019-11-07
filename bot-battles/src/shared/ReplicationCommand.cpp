#include "ReplicationCommand.h"

#include "MemoryStream.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
ReplicationCommand::ReplicationCommand()
    : m_replicationAction(ReplicationAction::CREATE_ENTITY)
{
}

//----------------------------------------------------------------------------------------------------
ReplicationCommand::ReplicationCommand(ReplicationAction replicationAction)
    : m_replicationAction(replicationAction)
{
}

//----------------------------------------------------------------------------------------------------
ReplicationCommand::~ReplicationCommand()
{
}

//----------------------------------------------------------------------------------------------------
void ReplicationCommand::Write(OutputMemoryStream& /*stream*/)
{
    /*
    stream.Write(m_replicationAction, GetRequiredBits<static_cast<U32>(ReplicationAction::COUNT)>::value);
    stream.Write(m_networkID);

    switch (m_replicationAction) {
    case ReplicationAction::CREATE_ENTITY:
    case ReplicationAction::UPDATE_ENTITY: {
        Entity entity = g_game->GetLinkingContext().GetEntity(m_networkID);
        Signature signature = g_game->GetEntityManager().GetSignature(entity);
        //stream.Write(signature); // TODO WRITE WELL THE SIGNATURE
        break;
    }

    default: {
        break;
    }
    }
	*/
}

//----------------------------------------------------------------------------------------------------
void ReplicationCommand::Read(InputMemoryStream& /*stream*/)
{
    /*
    stream.Read(m_replicationAction, GetRequiredBits<static_cast<U32>(ReplicationAction::COUNT)>::value);
    stream.Read(m_networkID);

    switch (m_replicationAction) {
    case ReplicationAction::CREATE_ENTITY:
    case ReplicationAction::UPDATE_ENTITY: {
        Entity entity = g_game->GetLinkingContext().GetEntity(m_networkID);
        Signature signature = g_game->GetEntityManager().GetSignature(entity);
        //stream.Read(signature); // TODO READ WELL THE SIGNATURE
        break;
    }

    default: {
        break;
    }
    }
	*/
}
}
