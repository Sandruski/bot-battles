#ifndef __REPLICATION_MANAGER_CLIENT_H__
#define __REPLICATION_MANAGER_CLIENT_H__

namespace sand {

class InputMemoryStream;
//----------------------------------------------------------------------------------------------------
class ReplicationManagerClient {
public:
    ReplicationManagerClient();
    ~ReplicationManagerClient();

    void Read(InputMemoryStream& inputStream) const;
    void ReadCreateEntityAction(InputMemoryStream& inputStream, NetworkID networkID) const;
    void ReadUpdateEntityAction(InputMemoryStream& inputStream, NetworkID networkID) const;
    void ReadRemoveEntityAction(InputMemoryStream& inputStream, NetworkID networkID) const;
};
}

#endif
