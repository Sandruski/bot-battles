#ifndef __CLIENT_COMPONENT_H__
#define __CLIENT_COMPONENT_H__

#include "Component.h"
#include "DeliveryManagerClient.h"
#include "Frame.h"
#include "Input.h"
#include "ReplicationManagerClient.h"
#include "TransformComponent.h"

namespace sand {

class SocketAddress;
class UDPSocket;
class TCPSocket;

//----------------------------------------------------------------------------------------------------
// System Component
struct ClientComponent : public Component {

    ClientComponent();

    void LoadFromConfig(const rapidjson::Value& value);

    bool IsConnected() const;
    bool IsLocalPlayer(Entity entity) const;

    void ConnectSockets();
    void DisconnectSockets();

    void Reset();

    ReplicationManagerClient m_replicationManager;
    DeliveryManagerClient m_deliveryManager;

    std::shared_ptr<UDPSocket> m_UDPSocket;
    std::shared_ptr<TCPSocket> m_TCPSocket;
    std::shared_ptr<SocketAddress> m_socketAddress;

    std::string m_ip;
    std::string m_port;

    std::string m_map;

    std::string m_name;

    PlayerID m_playerID;
    Entity m_entity;

    F32 m_lastPacketTime;

    U32 m_lastAckdFrame;
    U32 m_interpolationFromFrame;
    U32 m_interpolationToFrame;
    F32 m_interpolationPercentage;

    CircularBuffer<Frame, MAX_FRAMES> m_frameBuffer;

    CircularBuffer<Input, MAX_FRAMES> m_inputBuffer;
    bool m_isLastInputTransformPending;
    bool m_isLastInputWeaponPending;

    F32 m_RTT;

    bool m_isClientPrediction;
    bool m_isServerReconciliation;
    bool m_isEntityInterpolation;

    bool m_connectSockets;
    bool m_disconnectSockets;

    bool m_sendHelloPacket;
    bool m_sendReHelloPacket;
};
}

#endif
