#ifndef __REPLICATION_MANAGER_SERVER_H__
#define __REPLICATION_MANAGER_SERVER_H__

#include "Observer.h"
#include "ReplicationCommand.h"

namespace sand {

class OutputMemoryStream;
class Delivery;

//----------------------------------------------------------------------------------------------------
class ReplicationManagerServer : public Observer {
public:
    ReplicationManagerServer();
    ~ReplicationManagerServer();

    bool CreateCommand(NetworkID networkID, U32 dirtyState);
    void RemoveCommand(NetworkID networkID);
    void AddDirtyState(NetworkID networkID, U32 dirtyState);

    void Write(OutputMemoryStream& outputStream);
    U32 WriteCreateOrUpdateAction(OutputMemoryStream& outputStream, NetworkID networkID, U32 dirtyState) const;

    void OnNotify(const Event& event) override;

private: 
	std::unordered_map<NetworkID, ReplicationCommand> m_networkIDToReplicationCommand;
};
}

#endif
