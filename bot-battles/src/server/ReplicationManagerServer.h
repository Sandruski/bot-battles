#ifndef __REPLICATION_MANAGER_SERVER_H__
#define __REPLICATION_MANAGER_SERVER_H__

namespace sand {

class OutputMemoryStream;
class ReplicationCommand;

//----------------------------------------------------------------------------------------------------
class ReplicationManagerServer {
public:
    ReplicationManagerServer();
    ~ReplicationManagerServer();

    bool CreateCommand(NetworkID networkID, U32 dirtyState);
    void RemoveCommand(NetworkID networkID);
    void AddDirtyState(NetworkID networkID, U32 dirtyState);

    void Write(OutputMemoryStream& outputStream);
    U32 WriteCreateOrUpdateAction(OutputMemoryStream& outputStream, NetworkID networkID, U32 dirtyState) const;

private:
    std::unordered_map<NetworkID, ReplicationCommand> m_networkIDToReplicationCommand;
};
}

#endif
