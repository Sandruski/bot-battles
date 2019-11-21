#ifndef __SERVER_SYSTEM_H__
#define __SERVER_SYSTEM_H__

#include "System.h"

namespace sand {

class OutputMemoryStream;
class InputMemoryStream;
class SocketAddress;
class ClientProxy;

struct SingletonServerComponent;

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

    void AddDirtyState(NetworkID networkID, U32 dirtyState);

private:
    void SendOutgoingPackets(SingletonServerComponent& singletonServer) const;
    void SendWelcomePacket(const SingletonServerComponent& singletonServer, PlayerID playerID, const SocketAddress& toSocketAddress) const;
    void SendStatePacket(const SingletonServerComponent& singletonServer, std::shared_ptr<ClientProxy> clientProxy) const;
    bool SendPacket(const SingletonServerComponent& singletonServer, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const;

    void ReceiveIncomingPackets(SingletonServerComponent& singletonServer);
    void ReceivePacket(SingletonServerComponent& singletonServer, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress);
    void ReceiveHelloPacket(SingletonServerComponent& singletonServer, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress, PlayerID& playerID);
    void ReceiveInputPacket(SingletonServerComponent& singletonServer, InputMemoryStream& inputStream, PlayerID& playerID) const;

    void OnConnectionReset(const SocketAddress& fromSocketAddress) const;
    void OnDisconnect() const;
};
}

#endif
