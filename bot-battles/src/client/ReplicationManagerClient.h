#ifndef __REPLICATION_MANAGER_CLIENT_H__
#define __REPLICATION_MANAGER_CLIENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class ReplicationManagerClient : public Subject {
public:
    void Read(InputMemoryStream& inputStream);
    void ReadCreateAction(InputMemoryStream& inputStream, Entity& entity, NetworkID networkID, U32 frame) const;
    void ReadUpdateAction(InputMemoryStream& inputStream, Entity entity, U32 frame) const;
    void ReadRemoveAction(Entity entity) const;
};
}

#endif
