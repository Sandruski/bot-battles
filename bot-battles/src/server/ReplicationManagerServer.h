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
