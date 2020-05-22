#ifndef __REPLICATION_MANAGER_SERVER_H__
#define __REPLICATION_MANAGER_SERVER_H__

#include "ReplicationCommand.h"

namespace sand {

class ReplicationResultManager;

//----------------------------------------------------------------------------------------------------
class ReplicationManagerServer {
public:
    ReplicationManagerServer();

    bool AddCommand(NetworkID networkID, bool isReplicated, U32 dirtyState);
    bool RemoveCommand(NetworkID networkID);

    void SetCreate(NetworkID networkID, U32 dirtyState);
    void SetUpdate(NetworkID networkID);
    void SetRemove(NetworkID networkID);
    void SetIsReplicated(NetworkID networkID, bool isReplicated);
    void AddDirtyState(NetworkID networkID, U32 dirtyState);

    void Write(OutputMemoryStream& outputStream, ReplicationResultManager& replicationResultManager);
    U32 WriteCreateAction(OutputMemoryStream& outputStream, NetworkID networkID, U32 dirtyState) const;
    U32 WriteUpdateAction(OutputMemoryStream& outputStream, NetworkID networkID, U32 dirtyState) const;

    void Reset();

private:
    std::unordered_map<NetworkID, ReplicationCommand> m_networkIDToReplicationCommand;
};
}

#endif
