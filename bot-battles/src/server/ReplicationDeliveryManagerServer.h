#ifndef __REPLICATION_DELIVERY_MANAGER_SERVER_H__
#define __REPLICATION_DELIVERY_MANAGER_SERVER_H__

#include "ReplicationCommand.h"

namespace sand {

class DeliveryManager;
class ReplicationManagerServer;

//----------------------------------------------------------------------------------------------------
class ReplicationDeliveryManagerServer : public Observer {
public:
    ReplicationDeliveryManagerServer(std::shared_ptr<ReplicationManagerServer> replicationManagerServer);
    ~ReplicationDeliveryManagerServer();

    bool AddDelivery(NetworkID networkID, const ReplicationCommand& replicationCommand);

    void HandleDeliverySuccess(const DeliveryManager& deliveryManager) const;
    void HandleDeliveryFailure(const DeliveryManager& deliveryManager) const;

    //void OnNotify(const Event& event) override;

    const std::unordered_map<NetworkID, ReplicationCommand>& GetNetworkIDToReplicationCommandMap() const
    {
        return m_networkIDToReplicationCommand;
    }

private:
    void HandleCreateDeliverySuccess(NetworkID networkID) const;
    void HandleRemoveDeliverySuccess(NetworkID networkID) const;

    void HandleCreateDeliveryFailure(NetworkID networkID) const;
    void HandleUpdateDeliveryFailure(NetworkID networkID, U32 dirtyState, const DeliveryManager& deliveryManager) const;
    void HandleRemoveDeliveryFailure(NetworkID networkID) const;

private:
    std::shared_ptr<ReplicationManagerServer> m_replicationManagerServer;
    std::unordered_map<NetworkID, ReplicationCommand> m_networkIDToReplicationCommand;
};
}

#endif
