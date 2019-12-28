#ifndef __SERVER_SYSTEM_H__
#define __SERVER_SYSTEM_H__

#include "System.h"

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;
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
    ServerSystem();
    ~ServerSystem() override;

    bool StartUp() override;
    bool PreUpdate() override;
    bool Update() override;

    void OnNotify(const Event& event) override;

private:
    void SendOutgoingPackets(ServerComponent& serverComponent);
    void SendWelcomePacket(const ServerComponent& serverComponent, PlayerID playerID, const SocketAddress& toSocketAddress) const;
    void SendStatePacket(const ServerComponent& serverComponent, std::shared_ptr<ClientProxy> clientProxy) const;
    bool SendPacket(const ServerComponent& serverComponent, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const;

    void ReceiveIncomingPackets(ServerComponent& serverComponent);
    void ReceivePacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress);
    void ReceiveHelloPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress, PlayerID& playerID);
    void ReceiveInputPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, PlayerID& playerID) const;

    void ClientConnectionReset(ServerComponent& serverComponent, const SocketAddress& socketAddress);
    void DisconnectClient(ServerComponent& serverComponent, PlayerID playerID, Entity entity);
};
}

#endif
