#ifndef __REPLICATION_MANAGER_SERVER_H__
#define __REPLICATION_MANAGER_SERVER_H__

#include "ReplicationCommand.h"

namespace sand {

class ReplicationResultManager;

//----------------------------------------------------------------------------------------------------
class ReplicationManagerServer {
public:
    ReplicationManagerServer();

    bool AddCommand(NetworkID networkID, U64 dirtyState, bool isReplicated);
    bool RemoveCommand(NetworkID networkID);

    void SetCreate(NetworkID networkID, U64 dirtyState);
    void SetUpdate(NetworkID networkID);
    void SetRemove(NetworkID networkID);
    void SetIsReplicated(NetworkID networkID, bool isReplicated);
    void AddDirtyState(NetworkID networkID, U64 dirtyState);

    void Write(OutputMemoryStream& outputStream, ReplicationResultManager& replicationResultManager);
    U64 WriteCreateAction(OutputMemoryStream& outputStream, NetworkID networkID, U64 dirtyState) const;
    U64 WriteUpdateAction(OutputMemoryStream& outputStream, NetworkID networkID, U64 dirtyState) const;

    void Reset();

private:
    std::unordered_map<NetworkID, ReplicationCommand> m_networkIDToReplicationCommand;
};
}

#endif
