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
    bool IsLocalEntity(Entity entity) const;
    bool IsLocalPlayer(PlayerID playerID) const;

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
    std::string m_bot;

    PlayerID m_playerID;
    Entity m_entity;

    MyTimer m_incomingPacketsTimer;
    MyTimer m_outgoingPacketsTimer;

    F32 m_incomingPacketsTimeout;
    F32 m_outgoingPacketsTimeout;
    F32 m_lastPacketTime;

    U32 m_lastAckdFrame;
    U32 m_interpolationFromFrame;
    U32 m_interpolationToFrame;
    F32 m_interpolationPercentage;
    F32 m_entityInterpolationPeriod; // TODO: *2? https://developer.valvesoftware.com/wiki/Source_Multiplayer_Networking#Entity_interpolation

    CircularBuffer<Frame, MAX_FRAMES> m_frameBuffer; // client-side only
    /*
    (for Entity Interpolation)
    -Add: ReplicationManagerClient 
    -Remove: RemotePlayerMovementSystem
    */

    CircularBuffer<Input, MAX_INPUTS> m_inputBuffer;
    /*
    -Add: InputSystem
    -Remove: OutputSystemClient
    */
    bool m_isLastMovementInputPending;
    bool m_isLastWeaponInputPending;
    bool m_isLastHealthInputPending;

    F32 m_secondsBetweenConnect;
    MyTimer m_connectTimer;

    F32 m_RTT;

    F32 m_disconnectTimeout;

    U32 m_maxPacketsPerFrame;
    U32 m_maxInputsPerPaquet;

    bool m_isClientPredictionServerReconciliation;
    bool m_isClientPrediction;
    bool m_isServerReconciliation;
    bool m_isEntityInterpolation;
};
}

#endif
