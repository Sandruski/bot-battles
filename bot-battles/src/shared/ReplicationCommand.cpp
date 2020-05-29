#include "ReplicationCommand.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ReplicationCommand::ReplicationCommand()
    : m_replicationActionType(ReplicationActionType::CREATE)
    , m_dirtyState(0)
    , m_isReplicated(true)
    , m_wasReplicated(true)
{
}

//----------------------------------------------------------------------------------------------------
ReplicationCommand::ReplicationCommand(ReplicationActionType replicationAction, U64 dirtyState, bool isReplicated)
    : m_replicationActionType(replicationAction)
    , m_dirtyState(dirtyState)
    , m_isReplicated(isReplicated)
    , m_wasReplicated(isReplicated)
{
}

//----------------------------------------------------------------------------------------------------
void ReplicationCommand::SetIsReplicated(bool isReplicated)
{
    m_wasReplicated = m_isReplicated;
    m_isReplicated = isReplicated;
}

//----------------------------------------------------------------------------------------------------
void ReplicationCommand::SetWasReplicated(bool wasReplicated)
{
    m_wasReplicated = wasReplicated;
}

//----------------------------------------------------------------------------------------------------
bool ReplicationCommand::GetIsReplicated() const
{
    return m_isReplicated;
}

//----------------------------------------------------------------------------------------------------
bool ReplicationCommand::GetWasReplicated() const
{
    return m_wasReplicated;
}

//----------------------------------------------------------------------------------------------------
void ReplicationCommand::AddDirtyState(U64 dirtyState)
{
    m_dirtyState |= dirtyState;
}

//----------------------------------------------------------------------------------------------------
void ReplicationCommand::RemoveDirtyState(U64 dirtyState)
{
    m_dirtyState &= ~dirtyState;
}

//----------------------------------------------------------------------------------------------------
bool ReplicationCommand::HasDirtyState() const
{
    return (m_dirtyState > 0 || m_replicationActionType == ReplicationActionType::REMOVE);
}

//----------------------------------------------------------------------------------------------------
U64 ReplicationCommand::GetDirtyState() const
{
    return m_dirtyState;
}
}
