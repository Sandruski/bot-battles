#ifndef __REPLICATION_COMMAND_H__
#define __REPLICATION_COMMAND_H__

#include "ReplicationActionTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class ReplicationCommand {
public:
    ReplicationCommand();
    ReplicationCommand(ReplicationActionType replicationAction, U32 dirtyState, bool isReplicated);

    void SetIsReplicated(bool isReplicated);
    void SetWasReplicated(bool wasReplicated);
    bool GetIsReplicated() const;
    bool GetWasReplicated() const;

    void AddDirtyState(U32 dirtyState);
    void RemoveDirtyState(U32 dirtyState);
    bool HasDirtyState() const;
    U32 GetDirtyState() const;

public:
    ReplicationActionType m_replicationActionType;

private:
    U32 m_dirtyState;
    bool m_isReplicated;
    bool m_wasReplicated;
};
}

#endif
