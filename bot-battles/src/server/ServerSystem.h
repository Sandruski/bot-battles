#ifndef __SERVER_SYSTEM_H__
#define __SERVER_SYSTEM_H__

#include "ComponentDefs.h"
#include "System.h"

namespace sand {

struct SingletonServerComponent;
class OutputMemoryStream;
class InputMemoryStream;
class SocketAddress;
class ClientProxy;

//----------------------------------------------------------------------------------------------------
class ServerSystem : public System {
public:
    static SystemType GetType()
    {
        return SystemType::SERVER;
    }

public:
    ServerSystem();
    ~ServerSystem() override;

    bool StartUp() override;
    bool Update() override;
    bool ShutDown() override;

private:
    void SendWelcomePacket(const SingletonServerComponent& server, PlayerID playerID, const SocketAddress& toSocketAddress) const;
    bool SendPacket(const SingletonServerComponent& server, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const;

    void ReceivePacket(SingletonServerComponent& server, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress) const;
    void ReceiveHelloPacket(SingletonServerComponent& server, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress) const;
    void ReceiveInputPacket(SingletonServerComponent& server, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress) const;
};
}

#endif
