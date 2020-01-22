#ifndef __CLIENT_COMPONENT_H__
#define __CLIENT_COMPONENT_H__

#include "Component.h"
#include "DeliveryManagerClient.h"
#include "Moves.h"
#include "ReplicationManagerClient.h"

namespace sand {

class SocketAddress;
class UDPSocket;

//----------------------------------------------------------------------------------------------------
// System Component
struct ClientComponent : public Component {

    ClientComponent();

    void LoadFromConfig(const rapidjson::Value& value);

    bool IsConnected() const;
    bool IsLocalPlayer(Entity entity) const;

    ReplicationManagerClient m_replicationManager;
    DeliveryManagerClient m_deliveryManager;

    std::shared_ptr<UDPSocket> m_socket;
    std::shared_ptr<SocketAddress> m_socketAddress;

    std::string m_name;
    std::string m_ip;
    std::string m_port;

    PlayerID m_playerID;
    Entity m_entity;

    //std::deque<TransformComponent> m_moves;
    Moves m_moves;
    bool m_isLastMovePending;

    F32 m_RTT;

    bool m_isClientSidePrediction;
    bool m_isServerReconciliation;
    bool m_isEntityInterpolation;
    bool m_isLagCompensation;
};
}

#endif
