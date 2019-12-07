#include "ReplicationResultManager.h"

#include "ComponentMemberTypes.h"
#include "DeliveryManagerServer.h"
#include "GameServer.h"
#include "LinkingContext.h"
#include "ReplicationManagerServer.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
ReplicationResultManager::ReplicationResultManager(std::shared_ptr<ReplicationManagerServer> replicationManagerServer)
    : m_replicationManagerServer(std::move(replicationManagerServer)) // TODO: std move
    , m_networkIDToReplicationCommand()
{
}

//----------------------------------------------------------------------------------------------------
ReplicationResultManager::~ReplicationResultManager()
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

    m_networkIDToReplicationCommand[networkID] = replicationCommand;

    return true;
}

//----------------------------------------------------------------------------------------------------
void ReplicationResultManager::HandleDeliverySuccess(const DeliveryManagerServer& deliveryManagerServer) const
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
            U32 dirtyState = replicationCommand.GetDirtyState();
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
void ReplicationResultManager::HandleCreateDeliverySuccess(NetworkID networkID) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);
    if (entity != INVALID_ENTITY) {
        m_replicationManagerServer->SetUpdate(networkID);
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationResultManager::HandleRemoveDeliverySuccess(NetworkID networkID) const
{
    m_replicationManagerServer->RemoveCommand(networkID);
}

//----------------------------------------------------------------------------------------------------
void ReplicationResultManager::HandleCreateDeliveryFailure(NetworkID networkID) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);
    if (entity != INVALID_ENTITY) {
        m_replicationManagerServer->SetCreate(networkID, static_cast<U32>(ComponentMemberType::ALL));
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationResultManager::HandleUpdateDeliveryFailure(NetworkID networkID, U32 dirtyState, const DeliveryManagerServer& deliveryManagerServer) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);
    if (entity != INVALID_ENTITY) {
        const std::deque<Delivery>& deliveries = deliveryManagerServer.GetDeliveries();
        for (const Delivery& delivery : deliveries) {
            const std::unordered_map<NetworkID, ReplicationCommand>& networkIDToReplicationCommand = delivery.m_replicationResultManager->GetNetworkIDToReplicationCommandMap();
            for (const auto& pair : networkIDToReplicationCommand) {
                NetworkID otherNetworkID = pair.first;
                if (otherNetworkID == networkID) {
                    U32 otherDirtyState = pair.second.GetDirtyState();
                    dirtyState &= ~otherDirtyState;
                }
            }
        }

        m_replicationManagerServer->AddDirtyState(networkID, dirtyState);
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationResultManager::HandleRemoveDeliveryFailure(NetworkID networkID) const
{
    m_replicationManagerServer->SetRemove(networkID);
}
}
