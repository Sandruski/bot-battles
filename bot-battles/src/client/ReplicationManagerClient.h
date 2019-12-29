#ifndef __REPLICATION_MANAGER_CLIENT_H__
#define __REPLICATION_MANAGER_CLIENT_H__

namespace sand {

class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
class ReplicationManagerClient {
public:
    void Read(InputMemoryStream& inputStream) const;
    void ReadCreateAction(InputMemoryStream& inputStream, NetworkID networkID) const;
    void ReadUpdateAction(InputMemoryStream& inputStream, NetworkID networkID) const;
    void ReadRemoveAction(NetworkID networkID) const;
};
}

#endif
