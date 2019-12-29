#ifndef __CLIENT_SYSTEM_H__
#define __CLIENT_SYSTEM_H__

#include "System.h"

namespace sand {

struct ClientComponent;
struct MoveComponent;
class OutputMemoryStream;
class InputMemoryStream;

//----------------------------------------------------------------------------------------------------
class ClientSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::CLIENT;
    }

public:
    bool StartUp() override;
    bool Update() override;

private:
    void SendOutgoingPackets(ClientComponent& clientComponent) const;
    void UpdateSendHelloPacket(ClientComponent& clientComponent) const;
    void UpdateSendInputPacket(ClientComponent& clientComponent) const;

    bool SendHelloPacket(const ClientComponent& clientComponent) const;
    bool SendInputPacket(const ClientComponent& clientComponent, const MoveComponent& moveComponent) const;
    bool SendPacket(const ClientComponent& clientComponent, const OutputMemoryStream& outputStream) const;

    void ReceiveIncomingPackets(ClientComponent& clientComponent) const;
    void ReceivePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream) const;
    void ReceiveWelcomePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream) const;
    void ReceiveStatePacket(ClientComponent& clientComponent, InputMemoryStream& inputStream) const;

    void OnConnectionReset(ClientComponent& clientComponent) const;
    void OnDisconnect(ClientComponent& clientComponent) const;
};
}

#endif
