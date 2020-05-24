#ifndef __REPLICATION_MANAGER_CLIENT_H__
#define __REPLICATION_MANAGER_CLIENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class ReplicationManagerClient : public Subject {
public:
    void Read(InputMemoryStream& inputStream);
    void ReadCreateAction(InputMemoryStream& inputStream, NetworkID networkID, U32 frame) const;
    void ReadUpdateAction(InputMemoryStream& inputStream, NetworkID networkID, U32 frame) const;
    void ReadRemoveAction(NetworkID networkID) const;
};
}

#endif
