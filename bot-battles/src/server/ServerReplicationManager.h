#ifndef __SERVER_REPLICATION_MANAGER_H__
#define __SERVER_REPLICATION_MANAGER_H__

namespace sand {

class OutputMemoryStream;
class ReplicationCommand;

//----------------------------------------------------------------------------------------------------
class ServerReplicationManager {
public:
    ServerReplicationManager();
    ~ServerReplicationManager();

    bool CreateEntityCommand(NetworkID networkID);
    bool RemoveEntityCommand(NetworkID networkID);

    void Write(OutputMemoryStream& outputStream);
    void WriteCreateEntityAction(OutputMemoryStream& outputStream, NetworkID networkID) const;
    void WriteUpdateEntityAction(OutputMemoryStream& outputStream, NetworkID networkID) const;

private:
    std::unordered_map<NetworkID, ReplicationCommand> m_networkIDToReplicationCommand;
};
}

#endif
