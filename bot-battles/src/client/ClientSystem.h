#ifndef __CLIENT_SYSTEM_H__
#define __CLIENT_SYSTEM_H__

#include "System.h"

namespace sand {

struct ClientComponent;

//----------------------------------------------------------------------------------------------------
class ClientSystem : public System, public Subject, public Observer {
public:
    static SystemType GetType()
    {
        return SystemType::CLIENT;
    }

public:
    bool StartUp() override;
    bool PreUpdate() override;
    bool ShutDown() override;

    void OnNotify(const Event& event) override;

    void ReceiveIncomingPackets(std::weak_ptr<ClientComponent> clientComponent);
    void SendOutgoingPackets(std::weak_ptr<ClientComponent> clientComponent);

private:
    void ReceivePacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& inputStream);
    void ReceiveWelcomePacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& inputStream);
    void ReceiveUnWelcomePacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& inputStream);
    void ReceiveReWelcomePacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& inputStream);
    void ReceivePlayPacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& inputStream);
    void ReceiveResultsPacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& inputStream);
    void ReceiveByePacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& inputStream);
    void ReceiveStatePacket(std::weak_ptr<ClientComponent> clientComponent, InputMemoryStream& inputStream) const;

    bool SendHelloPacket(const std::weak_ptr<ClientComponent> clientComponent) const;
    bool SendReHelloPacket(const std::weak_ptr<ClientComponent> clientComponent) const;
    bool SendByePacket(std::weak_ptr<ClientComponent> clientComponent);
    bool SendInputPacket(std::weak_ptr<ClientComponent> clientComponent) const;
    bool SendUDPPacket(const std::weak_ptr<ClientComponent> clientComponent, const OutputMemoryStream& outputStream) const;
    bool SendTCPPacket(const std::weak_ptr<ClientComponent> clientComponent, const OutputMemoryStream& outputStream) const;

    bool ConnectSockets(std::weak_ptr<ClientComponent> clientComponent);
    bool CheckConnect(std::weak_ptr<ClientComponent> clientComponent);
    bool DisconnectSockets(std::weak_ptr<ClientComponent> clientComponent);

    void ConnectionReset(std::weak_ptr<ClientComponent> clientComponent);
    void Disconnect(std::weak_ptr<ClientComponent> clientComponent);
};
}

#endif
