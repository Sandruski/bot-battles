#ifndef __CLIENT_SYSTEM_H__
#define __CLIENT_SYSTEM_H__

#include "ComponentDefs.h"
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
    bool ShutDown() override;

private:
    void UpdateSendHelloPacket(SingletonClientComponent& client) const;
    void UpdateSendInputPacket(SingletonClientComponent& client) const;
    bool SendHelloPacket(const SingletonClientComponent& client) const;
    bool SendInputPacket(const SingletonClientComponent& client) const;
    bool SendPacket(const SingletonClientComponent& client, const OutputMemoryStream& outputStream) const;

    void ReceivePacket(SingletonClientComponent& client, InputMemoryStream& inputStream) const;
    void ReceiveWelcomePacket(SingletonClientComponent& client, InputMemoryStream& inputStream) const;
    void ReceiveStatePacket(SingletonClientComponent& client, InputMemoryStream& inputStream) const;
};
}

#endif
