#include "ReplicationManager.h"

#include "EntityManager.h"
#include "Game.h"
#include "LinkingContext.h"
#include "MemoryStream.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
ReplicationHeader::ReplicationHeader()
    : m_replicationAction(ReplicationAction::INVALID)
    , m_networkID(INVALID_NETWORK_ID)
{
}

//----------------------------------------------------------------------------------------------------
ReplicationHeader::ReplicationHeader(ReplicationAction replicationAction, NetworkID networkID)
    : m_replicationAction(replicationAction)
    , m_networkID(networkID)
{
}

//----------------------------------------------------------------------------------------------------
ReplicationHeader::~ReplicationHeader()
{
}

//----------------------------------------------------------------------------------------------------
void ReplicationHeader::Write(OutputMemoryStream& stream)
{
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
}

//----------------------------------------------------------------------------------------------------
void ReplicationHeader::Read(InputMemoryStream& stream)
{
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
}

//----------------------------------------------------------------------------------------------------
ReplicationManager::ReplicationManager()
{
}

//----------------------------------------------------------------------------------------------------
ReplicationManager::~ReplicationManager()
{
}

//----------------------------------------------------------------------------------------------------
void ReplicationManager::CreateEntityAction(OutputMemoryStream& stream, Entity entity) const
{
    NetworkID networkID = g_game->GetLinkingContext().AddEntity(entity);
    ReplicationHeader replicationHeader(ReplicationAction::CREATE_ENTITY, networkID);
    replicationHeader.Write(stream);
    // TODO: write all components
    // Note: if the entity has arrived here and has a networkID, it means that all of its components derive from NetComponent
    // TODO: this method can only be called by entities whose components derive from NetComponent
}

//----------------------------------------------------------------------------------------------------
void ReplicationManager::UpdateEntityAction(OutputMemoryStream& stream, Entity entity) const
{
    NetworkID networkID = g_game->GetLinkingContext().GetNetworkID(entity);
    ReplicationHeader replicationHeader(ReplicationAction::UPDATE_ENTITY, networkID);
    replicationHeader.Write(stream);
    // TODO: write all components
    // Note: if the entity has arrived here and has a networkID, it means that all of its components derive from NetComponent
    // TODO: this method can only be called by entities whose components derive from NetComponent
}

//----------------------------------------------------------------------------------------------------
void ReplicationManager::RemoveEntityAction(OutputMemoryStream& stream, Entity entity) const
{
    NetworkID networkID = g_game->GetLinkingContext().GetNetworkID(entity);
    ReplicationHeader replicationHeader(ReplicationAction::REMOVE_ENTITY, networkID);
    replicationHeader.Write(stream);
}

//----------------------------------------------------------------------------------------------------
void ReplicationManager::ProcessAction(InputMemoryStream& stream) const
{
    ReplicationHeader replicationHeader;
    replicationHeader.Read(stream);
}

//----------------------------------------------------------------------------------------------------
void ReplicationManager::CreateEntity(InputMemoryStream& /*stream*/, const ReplicationHeader& replicationHeader) const
{
    Entity entity = g_game->GetEntityManager().AddEntity();
    g_game->GetLinkingContext().AddEntity(entity, replicationHeader.GetNetworkID());
    // TODO: read all components
}

//----------------------------------------------------------------------------------------------------
void ReplicationManager::UpdateEntity(InputMemoryStream& stream, const ReplicationHeader& replicationHeader) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(replicationHeader.GetNetworkID());
    if (entity == INVALID_ENTITY) {
        U32 bitCount = replicationHeader.GetBitCount();
        WLOG("The entity has not been created yet. Advancing the memory stream's head %u bits...", bitCount);
        stream.AdvanceHead(bitCount);
        return;
    }

    // TODO: read all components
}

//----------------------------------------------------------------------------------------------------
void ReplicationManager::RemoveEntity(InputMemoryStream& /*stream*/, const ReplicationHeader& replicationHeader) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(replicationHeader.GetNetworkID());
    g_game->GetLinkingContext().RemoveEntity(entity);
    g_game->GetEntityManager().RemoveEntity(entity);
}
}
