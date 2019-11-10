#ifndef __CLIENT_REPLICATION_MANAGER_H__
#define __CLIENT_REPLICATION_MANAGER_H__

#include "EntityDefs.h"
#include "NetDefs.h"

namespace sand {

class InputMemoryStream;
class ReplicationCommand;

//----------------------------------------------------------------------------------------------------
class ClientReplicationManager {
public:
    ClientReplicationManager();
    ~ClientReplicationManager();

    void ReadActions(InputMemoryStream& inputStream) const;
    void ReadCreateEntityAction(InputMemoryStream& inputStream, NetworkID networkID) const;
    void ReadUpdateEntityAction(InputMemoryStream& inputStream, NetworkID networkID) const;
    void ReadRemoveEntityAction(InputMemoryStream& inputStream, NetworkID networkID) const;
};
}

#endif
