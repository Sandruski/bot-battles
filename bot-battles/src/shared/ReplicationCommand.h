#ifndef __REPLICATION_COMMAND_H__
#define __REPLICATION_COMMAND_H__

#include "ReplicationActionTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class ReplicationCommand {
public:
    ReplicationCommand();
    ReplicationCommand(ReplicationActionType replicationAction, bool isReplicated, U32 dirtyState);

    void AddDirtyState(U32 dirtyState);
    void RemoveDirtyState(U32 dirtyState);
    bool HasDirtyState() const;
    U32 GetDirtyState() const;

public:
    ReplicationActionType m_replicationActionType;
    bool m_isReplicated;

private:
    U32 m_dirtyState;
};
}

#endif
