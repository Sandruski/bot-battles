#include "ReplicationResultManager.h"

#include "ComponentMemberTypes.h"
#include "DeliveryManagerServer.h"
#include "GameServer.h"
#include "LinkingContext.h"
#include "ReplicationCommand.h"
#include "ReplicationManagerServer.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ReplicationResultManager::ReplicationResultManager(std::weak_ptr<ReplicationManagerServer> replicationManager)
    : m_replicationManager(std::move(replicationManager))
    , m_networkIDToReplicationCommand()
{
}

//----------------------------------------------------------------------------------------------------
bool ReplicationResultManager::AddDelivery(NetworkID networkID, const ReplicationCommand& replicationCommand)
{
    auto it = m_networkIDToReplicationCommand.find(networkID);
    if (it != m_networkIDToReplicationCommand.end()) {
        WLOG("NetworkID %u is already registered", networkID);
        return false;
    }

    m_networkIDToReplicationCommand.insert(std::make_pair(networkID, replicationCommand));

    return true;
}

//----------------------------------------------------------------------------------------------------
void ReplicationResultManager::HandleDeliverySuccess(const DeliveryManagerServer& /*deliveryManagerServer*/) const
{
    for (const auto& pair : m_networkIDToReplicationCommand) {

        NetworkID networkID = pair.first;
        ReplicationCommand replicationCommand = pair.second;
        switch (replicationCommand.m_replicationActionType) {
        case ReplicationActionType::CREATE: {
            HandleCreateDeliverySuccess(networkID);
            break;
        }

        case ReplicationActionType::REMOVE: {
            HandleRemoveDeliverySuccess(networkID);
            break;
        }

        default: {
            break;
        }
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationResultManager::HandleDeliveryFailure(const DeliveryManagerServer& deliveryManagerServer) const
{
    for (const auto& pair : m_networkIDToReplicationCommand) {
        NetworkID networkID = pair.first;
        ReplicationCommand replicationCommand = pair.second;
        switch (replicationCommand.m_replicationActionType) {
        case ReplicationActionType::CREATE: {
            HandleCreateDeliveryFailure(networkID);
            break;
        }

        case ReplicationActionType::UPDATE: {
            U64 dirtyState = replicationCommand.GetDirtyState();
            HandleUpdateDeliveryFailure(networkID, dirtyState, deliveryManagerServer);
            break;
        }

        case ReplicationActionType::REMOVE: {
            HandleRemoveDeliveryFailure(networkID);
            break;
        }

        default: {
            break;
        }
        }
    }
}

//----------------------------------------------------------------------------------------------------
const std::unordered_map<NetworkID, ReplicationCommand>& ReplicationResultManager::GetNetworkIDToReplicationCommandMap() const
{
    return m_networkIDToReplicationCommand;
}

//----------------------------------------------------------------------------------------------------
void ReplicationResultManager::HandleCreateDeliverySuccess(NetworkID networkID) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);
    if (entity < INVALID_ENTITY) {
        m_replicationManager.lock()->SetUpdate(networkID);
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationResultManager::HandleRemoveDeliverySuccess(NetworkID networkID) const
{
    m_replicationManager.lock()->RemoveCommand(networkID);
}

//----------------------------------------------------------------------------------------------------
void ReplicationResultManager::HandleCreateDeliveryFailure(NetworkID networkID) const
{
    ILOG("Resending create");

    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);
    if (entity < INVALID_ENTITY) {
        m_replicationManager.lock()->SetCreate(networkID, static_cast<U64>(ComponentMemberType::ALL));
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationResultManager::HandleUpdateDeliveryFailure(NetworkID networkID, U64 dirtyState, const DeliveryManagerServer& deliveryManagerServer) const
{
    ILOG("Resending update");

    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);
    if (entity < INVALID_ENTITY) {
        const std::deque<Delivery>& deliveries = deliveryManagerServer.GetDeliveries();
        for (const Delivery& delivery : deliveries) {
            const std::unordered_map<NetworkID, ReplicationCommand>& networkIDToReplicationCommand = delivery.m_replicationResultManager->GetNetworkIDToReplicationCommandMap();
            for (const auto& pair : networkIDToReplicationCommand) {
                NetworkID otherNetworkID = pair.first;
                if (otherNetworkID == networkID) {
                    U64 otherDirtyState = pair.second.GetDirtyState();
                    dirtyState &= ~otherDirtyState;
                }
            }
        }

        if (dirtyState > 0) {
            m_replicationManager.lock()->AddDirtyState(networkID, dirtyState);
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationResultManager::HandleRemoveDeliveryFailure(NetworkID networkID) const
{
    ILOG("Resending remove");

    m_replicationManager.lock()->SetRemove(networkID);
}
}
