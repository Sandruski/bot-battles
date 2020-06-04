#ifndef __SERVER_SYSTEM_H__
#define __SERVER_SYSTEM_H__

#include "System.h"

namespace sand {

class SocketAddress;
class ClientProxy;
struct ServerComponent;

//----------------------------------------------------------------------------------------------------
class ServerSystem : public System, public Subject, public Observer {
public:
    static SystemType GetType()
    {
        return SystemType::SERVER;
    }

public:
    bool StartUp() override;
    bool PreUpdate() override;
    bool ShutDown() override;

    void OnNotify(const Event& event) override;

    void ReceiveIncomingPackets(std::weak_ptr<ServerComponent> serverComponent);
    void SendOutgoingPackets(std::weak_ptr<ServerComponent> serverComponent);

private:
    void ReceivePacket(std::weak_ptr<ServerComponent> serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress);
    void ReceiveHelloPacket(std::weak_ptr<ServerComponent> serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress, PlayerID& playerID);
    void ReceiveReHelloPacket(std::weak_ptr<ServerComponent> serverComponent, InputMemoryStream& inputStream, PlayerID& playerID) const;
    void ReceiveByePacket(std::weak_ptr<ServerComponent> serverComponent, InputMemoryStream& inputStream, PlayerID& playerID);
    void ReceiveInputPacket(std::weak_ptr<ServerComponent> serverComponent, InputMemoryStream& inputStream, PlayerID& playerID) const;

    void SendWelcomePacket(const std::weak_ptr<ServerComponent> serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const;
    void SendUnWelcomePacket(std::weak_ptr<ServerComponent> serverComponent, const SocketAddress& toSocketAddress) const;
    void SendReWelcomePacket(const std::weak_ptr<ServerComponent> serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const;
    void SendPlayPacket(const std::weak_ptr<ServerComponent> serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const;
    void SendResultsPacket(const std::weak_ptr<ServerComponent> serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const;
    void SendByePacket(std::weak_ptr<ServerComponent> serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy);
    void SendStatePacket(const std::weak_ptr<ServerComponent> serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const;

    bool SendUDPPacket(const std::weak_ptr<ServerComponent> serverComponent, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const;
    bool SendTCPPacket(const std::weak_ptr<ServerComponent> serverComponent, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const;

    bool ConnectSockets(std::weak_ptr<ServerComponent> serverComponent);
    bool DisconnectSockets(std::weak_ptr<ServerComponent> serverComponent);

    void ConnectionReset(std::weak_ptr<ServerComponent> serverComponent, const SocketAddress& socketAddress);
    void Disconnect(std::weak_ptr<ServerComponent> serverComponent, PlayerID playerID, Entity entity);

    void OnPlayerAdded(PlayerID playerID) const;
    void OnNetworkEntityAdded(NetworkID networkID) const;
    void OnNetworkEntityRemoved(NetworkID networkID) const;
    void OnComponentMemberChanged(U64 dirtyState, Entity entity) const;
    void OnSeenNewEntity(PlayerID playerID, Entity entity) const;
    void OnSeenLostEntity(PlayerID playerID, Entity entity) const;
};
}

#endif
