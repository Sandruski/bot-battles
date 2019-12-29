#include "ReplicationCommand.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ReplicationCommand::ReplicationCommand()
    : m_replicationActionType(ReplicationActionType::CREATE)
    , m_dirtyState(0)
{
}

//----------------------------------------------------------------------------------------------------
ReplicationCommand::ReplicationCommand(ReplicationActionType replicationAction, U32 dirtyState)
    : m_replicationActionType(replicationAction)
    , m_dirtyState(dirtyState)
{
}

//----------------------------------------------------------------------------------------------------
void ReplicationCommand::AddDirtyState(U32 dirtyState)
{
    m_dirtyState |= dirtyState;
}

//----------------------------------------------------------------------------------------------------
void ReplicationCommand::RemoveDirtyState(U32 dirtyState)
{
    m_dirtyState &= ~dirtyState;
}

//----------------------------------------------------------------------------------------------------
bool ReplicationCommand::HasDirtyState() const
{
    return (m_dirtyState > 0 || m_replicationActionType == ReplicationActionType::REMOVE);
}

//----------------------------------------------------------------------------------------------------
U32 ReplicationCommand::GetDirtyState() const
{
    return m_dirtyState;
}
}
