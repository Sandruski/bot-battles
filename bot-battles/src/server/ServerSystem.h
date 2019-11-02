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
    enum class PacketType {
        HELLO,
        WELCOME,
        STATE,

        COUNT,
        INVALID
    };

public:
    ServerSystem();
    ~ServerSystem() override;

    bool StartUp() override;
    bool Update() override;
    bool ShutDown() override;

private:
    void SendWelcomePacket(const SingletonServerComponent& server, const ClientProxy& clientProxy) const;

    //void SendPacket(const SingletonClientComponent& client, const OutputMemoryStream& stream) const;
    void ReceivePacket(SingletonServerComponent& server, InputMemoryStream& stream, const SocketAddress& fromAddress) const;
    void ReceivePacketFromNewClient(SingletonServerComponent& server, InputMemoryStream& stream, const SocketAddress& fromAddress) const;
    void ReceivePacketFromExistingClient(SingletonServerComponent& server, InputMemoryStream& stream, const SocketAddress& fromAddress) const;
};
}

#endif
