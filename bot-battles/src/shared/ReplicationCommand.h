#ifndef __REPLICATION_COMMAND_H__
#define __REPLICATION_COMMAND_H__

#include "ReplicationActionTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class ReplicationCommand {
public:
    ReplicationCommand();
    ReplicationCommand(ReplicationActionType replicationAction, U64 dirtyState, bool isReplicated);

    void SetIsReplicated(bool isReplicated);
    void SetWasReplicated(bool wasReplicated);
    bool GetIsReplicated() const;
    bool GetWasReplicated() const;

    void AddDirtyState(U64 dirtyState);
    void RemoveDirtyState(U64 dirtyState);
    bool HasDirtyState() const;
    U64 GetDirtyState() const;

public:
    ReplicationActionType m_replicationActionType;

private:
    U64 m_dirtyState;
    bool m_isReplicated;
    bool m_wasReplicated;
};
}

#endif
