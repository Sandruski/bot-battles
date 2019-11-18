#ifndef __CLIENT_SYSTEM_H__
#define __CLIENT_SYSTEM_H__

#include "System.h"

namespace sand {

struct SingletonClientComponent;
struct SingletonInputComponent;
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
    void SendOutgoingPackets(SingletonClientComponent& singletonClient) const;
    void UpdateSendHelloPacket(SingletonClientComponent& singletonClient) const;
    void UpdateSendInputPacket(SingletonClientComponent& singletonClient) const;
    bool SendHelloPacket(const SingletonClientComponent& singletonClient) const;
    bool SendInputPacket(const SingletonClientComponent& singletonClient, const SingletonInputComponent& singletonInput) const;
    bool SendPacket(const SingletonClientComponent& singletonClient, const OutputMemoryStream& outputStream) const;

    void ReceiveIncomingPackets(SingletonClientComponent& singletonClient) const;
    void ReceivePacket(SingletonClientComponent& singletonClient, InputMemoryStream& inputStream) const;
    void ReceiveWelcomePacket(SingletonClientComponent& singletonClient, InputMemoryStream& inputStream) const;
    void ReceiveStatePacket(SingletonClientComponent& singletonClient, InputMemoryStream& inputStream) const;

    void OnConnectionReset(SingletonClientComponent& singletonClient) const;
    void OnDisconnect(SingletonClientComponent& singletonClient) const;
};
}

#endif
