#ifndef __REPLICATION_COMMAND_H__
#define __REPLICATION_COMMAND_H__

#include "ReplicationActionTypes.h"

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
class ReplicationCommand {
public:
    ReplicationCommand();
    ReplicationCommand(ReplicationActionType replicationAction, U32 dirtyState);

    void AddDirtyState(U32 dirtyState);
    void RemoveDirtyState(U32 dirtyState);
    bool HasDirtyState() const;
    U32 GetDirtyState() const;

public:
    ReplicationActionType m_replicationActionType;

private:
    U32 m_dirtyState;
};
}

#endif
