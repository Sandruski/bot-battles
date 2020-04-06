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

    void OnNotify(const Event& event) override;

    void ReceiveIncomingPackets(ServerComponent& serverComponent);
    void SendOutgoingPackets(ServerComponent& serverComponent);

private:
    void ReceivePacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress);
    void ReceiveHelloPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress, PlayerID& playerID);
    void ReceiveReHelloPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, PlayerID& playerID) const;
    void ReceiveByePacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, PlayerID& playerID);
    void ReceiveInputPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, PlayerID& playerID) const;

    void SendWelcomePacket(const ServerComponent& serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const;
    void SendUnWelcomePacket(ServerComponent& serverComponent, const SocketAddress& toSocketAddress) const;
    void SendReWelcomePacket(const ServerComponent& serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const;
    void SendPlayPacket(const ServerComponent& serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const;
    void SendResultsPacket(const ServerComponent& serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const;
    void SendByePacket(ServerComponent& serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy);
    void SendStatePacket(const ServerComponent& serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const;

    bool SendUDPPacket(const ServerComponent& serverComponent, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const;
    bool SendTCPPacket(const ServerComponent& serverComponent, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const;

    bool ConnectSockets(ServerComponent& serverComponent);
    bool DisconnectSockets(ServerComponent& serverComponent);

    void ConnectionReset(ServerComponent& serverComponent, const SocketAddress& socketAddress);
    void Disconnect(ServerComponent& serverComponent, PlayerID playerID, Entity entity);

    void OnPlayerAdded(PlayerID playerID) const;
    void OnNetworkEntityAdded(NetworkID networkID) const;
    void OnNetworkEntityRemoved(NetworkID networkID) const;
    void OnComponentMemberChanged(U32 dirtyState, Entity entity) const;
};
}

#endif
