#ifndef __CLIENT_SYSTEM_H__
#define __CLIENT_SYSTEM_H__

#include "System.h"

namespace sand {

struct SingletonClientComponent;
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
    ClientSystem();
    ~ClientSystem() override;

    bool StartUp() override;
    bool Update() override;

private:
    void SendOutgoingPackets(SingletonClientComponent& client) const;
    void UpdateSendHelloPacket(SingletonClientComponent& client) const;
    void UpdateSendInputPacket(SingletonClientComponent& client) const;
    bool SendHelloPacket(const SingletonClientComponent& client) const;
    bool SendInputPacket(const SingletonClientComponent& client) const;
    bool SendPacket(const SingletonClientComponent& client, const OutputMemoryStream& outputStream) const;

    void ReceiveIncomingPackets(SingletonClientComponent& client) const;
    void ReceivePacket(SingletonClientComponent& client, InputMemoryStream& inputStream) const;
    void ReceiveWelcomePacket(SingletonClientComponent& client, InputMemoryStream& inputStream) const;
    void ReceiveStatePacket(SingletonClientComponent& client, InputMemoryStream& inputStream) const;

    void OnConnectionReset(SingletonClientComponent& client) const;
    void OnDisconnect(SingletonClientComponent& client) const;
};
}

#endif
