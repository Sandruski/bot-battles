#ifndef __REPLICATION_RESULT_MANAGER_H__
#define __REPLICATION_RESULT_MANAGER_H__

#include "ReplicationCommand.h"

namespace sand {

class DeliveryManagerServer;
class ReplicationManagerServer;

//----------------------------------------------------------------------------------------------------
class ReplicationResultManager {
public:
    ReplicationResultManager(std::weak_ptr<ReplicationManagerServer> replicationManager);

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
    std::weak_ptr<ReplicationManagerServer> m_replicationManager;
    std::unordered_map<NetworkID, ReplicationCommand> m_networkIDToReplicationCommand;
};
}

#endif
