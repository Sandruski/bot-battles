#ifndef __CLIENT_SYSTEM_H__
#define __CLIENT_SYSTEM_H__

#include "System.h"

namespace sand {

struct ClientComponent;

//----------------------------------------------------------------------------------------------------
class ClientSystem : public System, public Subject {
public:
    static SystemType GetType()
    {
        return SystemType::CLIENT;
    }

public:
    bool StartUp() override;
    bool PreUpdate() override;

    bool ConnectSockets(ClientComponent& clientComponent);
    bool DisconnectSockets(ClientComponent& clientComponent);
    void ReceiveIncomingPackets(ClientComponent& clientComponent);
    void SendOutgoingPackets(ClientComponent& clientComponent);

private:
    void ReceivePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream);
    void ReceiveWelcomePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream);
    void ReceiveReWelcomePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream);
    void ReceiveStatePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream) const;
    void ReceiveResultPacket(ClientComponent& clientComponent, InputMemoryStream& inputStream);

    bool SendHelloPacket(const ClientComponent& clientComponent) const;
    bool SendReHelloPacket(const ClientComponent& clientComponent) const;
    bool SendInputPacket(ClientComponent& clientComponent) const;
    bool SendUDPPacket(const ClientComponent& clientComponent, const OutputMemoryStream& outputStream) const;
    bool SendTCPPacket(const ClientComponent& clientComponent, const OutputMemoryStream& outputStream) const;

    void ConnectionReset(ClientComponent& clientComponent);
    void Disconnect(ClientComponent& clientComponent);
};
}

#endif
