#ifndef __REPLICATION_RESULT_MANAGER_H__
#define __REPLICATION_RESULT_MANAGER_H__

namespace sand {

class DeliveryManagerServer;
class ReplicationManagerServer;
class ReplicationCommand;

//----------------------------------------------------------------------------------------------------
class ReplicationResultManager {
public:
    ReplicationResultManager(std::shared_ptr<ReplicationManagerServer> replicationManagerServer);
    ~ReplicationResultManager();

    bool AddDelivery(NetworkID networkID, const ReplicationCommand& replicationCommand);

    void HandleDeliverySuccess(const DeliveryManagerServer& deliveryManagerServer) const;
    void HandleDeliveryFailure(const DeliveryManagerServer& deliveryManagerServer) const;

    const std::unordered_map<NetworkID, ReplicationCommand>& GetNetworkIDToReplicationCommandMap() const;

private:
    void HandleCreateDeliverySuccess(NetworkID networkID) const;
    void HandleRemoveDeliverySuccess(NetworkID networkID) const;

    void HandleCreateDeliveryFailure(NetworkID networkID) const;
    void HandleUpdateDeliveryFailure(NetworkID networkID, U32 dirtyState, const DeliveryManagerServer& deliveryManagerServer) const;
    void HandleRemoveDeliveryFailure(NetworkID networkID) const;

private:
    std::shared_ptr<ReplicationManagerServer> m_replicationManagerServer;
    std::unordered_map<NetworkID, ReplicationCommand> m_networkIDToReplicationCommand;
};
}

#endif
