#include "ReplicationDeliveryManagerServer.h"

#include "ComponentMemberTypes.h"
#include "DeliveryManager.h"
#include "GameServer.h"
#include "LinkingContext.h"
#include "ReplicationManagerServer.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
ReplicationDeliveryManagerServer::ReplicationDeliveryManagerServer(std::shared_ptr<ReplicationManagerServer> replicationManagerServer)
    : m_replicationManagerServer(replicationManagerServer)
    , m_networkIDToReplicationCommand()
{
}

//----------------------------------------------------------------------------------------------------
ReplicationDeliveryManagerServer::~ReplicationDeliveryManagerServer()
{
}

//----------------------------------------------------------------------------------------------------
bool ReplicationDeliveryManagerServer::AddDelivery(NetworkID networkID, const ReplicationCommand& replicationCommand)
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
void ReplicationDeliveryManagerServer::HandleDeliverySuccess(const DeliveryManager& deliveryManager) const
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
void ReplicationDeliveryManagerServer::HandleDeliveryFailure(const DeliveryManager& deliveryManager) const
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
            HandleUpdateDeliveryFailure(networkID, dirtyState, deliveryManager);
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
void ReplicationDeliveryManagerServer::HandleCreateDeliverySuccess(NetworkID networkID) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);
    if (entity != INVALID_ENTITY) {
        m_replicationManagerServer->SetUpdate(networkID);
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationDeliveryManagerServer::HandleRemoveDeliverySuccess(NetworkID networkID) const
{
    m_replicationManagerServer->RemoveCommand(networkID);
}

//----------------------------------------------------------------------------------------------------
void ReplicationDeliveryManagerServer::HandleCreateDeliveryFailure(NetworkID networkID) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);
    if (entity != INVALID_ENTITY) {
        m_replicationManagerServer->SetCreate(networkID, static_cast<U32>(ComponentMemberType::ALL));
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationDeliveryManagerServer::HandleUpdateDeliveryFailure(NetworkID networkID, U32 dirtyState, const DeliveryManager& deliveryManager) const
{
    Entity entity = g_game->GetLinkingContext().GetEntity(networkID);
    if (entity != INVALID_ENTITY) {
        const std::deque<Delivery>& deliveries = deliveryManager.GetDeliveries();
        for (const Delivery& delivery : deliveries) {
            const std::unordered_map<NetworkID, ReplicationCommand>& networkIDToReplicationCommand = delivery.m_replicationDeliveryManager->GetNetworkIDToReplicationCommandMap();
            for (const auto& pair : networkIDToReplicationCommand) {
                U32 otherDirtyState = pair.second.GetDirtyState();
                dirtyState &= ~otherDirtyState;
            }
        }

        m_replicationManagerServer->AddDirtyState(networkID, dirtyState);
    }
}

//----------------------------------------------------------------------------------------------------
void ReplicationDeliveryManagerServer::HandleRemoveDeliveryFailure(NetworkID networkID) const
{
    m_replicationManagerServer->SetRemove(networkID);
}
}
