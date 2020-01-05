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
    void OnNotify(const Event& event) override;

    bool StartUp() override;
    bool PreUpdate() override;
    bool Update() override;

private:
    void SendOutgoingPackets(ServerComponent& serverComponent);
    void SendWelcomePacket(const ServerComponent& serverComponent, PlayerID playerID, const SocketAddress& toSocketAddress) const;
    void SendStatePacket(const ServerComponent& serverComponent, std::shared_ptr<ClientProxy> clientProxy) const;
    bool SendPacket(const ServerComponent& serverComponent, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const;

    void ReceiveIncomingPackets(ServerComponent& serverComponent);
    void ReceivePacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress);
    void ReceiveHelloPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress, PlayerID& playerID);
    void ReceiveInputPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, PlayerID& playerID) const;

    void ConnectionReset(ServerComponent& serverComponent, const SocketAddress& socketAddress);
    void Disconnect(ServerComponent& serverComponent, PlayerID playerID, Entity entity);

    void OnEntityAdded(Entity entity) const;
    void OnEntityRemoved(Entity entity) const;
    void OnComponentMemberChanged(U32 dirtyState, Entity entity) const;
};
}

#endif
