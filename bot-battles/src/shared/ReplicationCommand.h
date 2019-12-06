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
    ~ReplicationCommand();

    void AddDirtyState(U32 dirtyState);
    void RemoveDirtyState(U32 dirtyState);
    U32 GetDirtyState() const;
    bool HasDirtyState() const;

public:
    ReplicationActionType m_replicationActionType;

private:
    U32 m_dirtyState;
};
}

#endif
