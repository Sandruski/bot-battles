#include "ServerSystem.h"

#include "ClientProxy.h"
#include "ComponentMemberTypes.h"
#include "DeliveryManagerServer.h"
#include "GameServer.h"
#include "LinkingContext.h"
#include "MemoryStream.h"
#include "MessageTypes.h"
#include "ReplicationManagerServer.h"
#include "ReplicationResultManager.h"
#include "SingletonServerComponent.h"
#include "UDPSocket.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ServerSystem::ServerSystem()
{
}

//----------------------------------------------------------------------------------------------------
ServerSystem::~ServerSystem()
{
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::StartUp()
{
    WORD winsockVersion = MAKEWORD(2, 2);
    WSADATA winsockData;
    int iResult = WSAStartup(winsockVersion, &winsockData);
    if (iResult == SOCKET_ERROR) {
        NETLOG("WSAStartup");
        return false;
    }

    std::shared_ptr<SingletonServerComponent> singletonServer = g_gameServer->GetSingletonServerComponent();
    singletonServer->m_socketAddress = SocketAddress::CreateIPv4(INADDR_ANY, static_cast<U16>(atoi("9999")));
    assert(singletonServer->m_socketAddress != nullptr);
    singletonServer->m_socket = UDPSocket::CreateIPv4();
    assert(singletonServer->m_socket != nullptr);
    singletonServer->m_socket->SetNonBlockingMode(true);
    singletonServer->m_socket->Bind(*singletonServer->m_socketAddress);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::PreUpdate()
{
    NotifyEvents();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::Update()
{
    std::shared_ptr<SingletonServerComponent> singletonServer = g_gameServer->GetSingletonServerComponent();

    ReceiveIncomingPackets(*singletonServer);
    // TODO: ProcessQueuedPackets()
    SendOutgoingPackets(*singletonServer);

    return true;
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::PLAYER_ADDED: {
        std::shared_ptr<SingletonServerComponent> singletonServer = g_gameServer->GetSingletonServerComponent();
        std::shared_ptr<ClientProxy> clientProxy = singletonServer->GetClientProxyFromPlayerID(event.server.playerID);
        const std::unordered_map<NetworkID, Entity>& networkIDToEntity = g_gameServer->GetLinkingContext().GetNetworkIDToEntityMap();
        for (const auto& pair : networkIDToEntity) {
            clientProxy->m_replicationManager->AddCommand(pair.first, static_cast<U32>(ComponentMemberType::ALL));
        }
        break;
    }

    case EventType::ENTITY_ADDED: {
        NetworkID networkID = g_gameServer->GetLinkingContext().GetNetworkID(event.entity.entity);
        std::shared_ptr<SingletonServerComponent> singletonServer = g_gameServer->GetSingletonServerComponent();
        const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = singletonServer->GetPlayerIDToClientProxyMap();
        for (const auto& pair : playerIDToClientProxy) {
            pair.second->m_replicationManager->AddCommand(networkID, static_cast<U32>(ComponentMemberType::ALL));
        }
        break;
    }

    case EventType::COMPONENT_MEMBER_CHANGED: {
        NetworkID networkID = g_gameServer->GetLinkingContext().GetNetworkID(event.component.entity);
        std::shared_ptr<SingletonServerComponent> singletonServer = g_gameServer->GetSingletonServerComponent();
        const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = singletonServer->GetPlayerIDToClientProxyMap();
        for (const auto& pair : playerIDToClientProxy) {
            pair.second->m_replicationManager->AddDirtyState(networkID, event.component.dirtyState);
        }
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendOutgoingPackets(SingletonServerComponent& singletonServer) const
{
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = singletonServer.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        std::shared_ptr<ClientProxy> clientProxy = pair.second;
        clientProxy->m_deliveryManager->ProcessTimedOutPackets();

        //if (clientProxy->m_isLastMoveTimestampDirty) {
        SendStatePacket(singletonServer, clientProxy);
        //}
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendWelcomePacket(const SingletonServerComponent& singletonServer, PlayerID playerID, const SocketAddress& toSocketAddress) const
{
    OutputMemoryStream welcomePacket;
    welcomePacket.Write(ServerMessageType::WELCOME);
    const bool isSuccessful = playerID != INVALID_PLAYER_ID;
    welcomePacket.Write(isSuccessful);
    if (isSuccessful) {
        welcomePacket.Write(playerID);
    }

    ILOG("Sending welcome packet to player %u...", playerID);

    const bool result = SendPacket(singletonServer, welcomePacket, toSocketAddress);
    if (result) {
        ILOG("Welcome packet successfully sent to player %u", playerID);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendStatePacket(const SingletonServerComponent& singletonServer, std::shared_ptr<ClientProxy> clientProxy) const
{
    OutputMemoryStream statePacket;
    statePacket.Write(ServerMessageType::STATE);
    Delivery& delivery = clientProxy->m_deliveryManager->WriteState(statePacket);
    delivery.m_replicationResultManager = std::make_shared<ReplicationResultManager>(clientProxy->m_replicationManager);
    clientProxy->m_replicationManager->Write(statePacket, *delivery.m_replicationResultManager);

    const char* name = clientProxy->GetName();
    ILOG("Sending state packet to player %s...", name);

    const bool result = SendPacket(singletonServer, statePacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("State packet successfully sent to player %s", name);
    }
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::SendPacket(const SingletonServerComponent& singletonServer, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const
{
    return singletonServer.m_socket->SendTo(outputStream.GetPtr(), outputStream.GetByteLength(), toSocketAddress);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveIncomingPackets(SingletonServerComponent& singletonServer)
{
    InputMemoryStream packet;
    SocketAddress fromSocketAddress;

    U32 receivedPacketCount = 0;

    while (receivedPacketCount < MAX_PACKETS_PER_FRAME) {
        I32 readByteCount = singletonServer.m_socket->ReceiveFrom(packet.GetPtr(), packet.GetByteCapacity(), fromSocketAddress);
        if (readByteCount > 0) {
            packet.SetCapacity(readByteCount);
            packet.ResetHead();
            ReceivePacket(singletonServer, packet, fromSocketAddress);

            ++receivedPacketCount;
        } else if (readByteCount == -WSAECONNRESET) {
            OnConnectionReset(fromSocketAddress);
        } else if (readByteCount == 0 || -WSAEWOULDBLOCK) {
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceivePacket(SingletonServerComponent& singletonServer, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress)
{
    ClientMessageType type;
    inputStream.Read(type);
    PlayerID playerID = INVALID_PLAYER_ID;
    switch (type) {
    case ClientMessageType::HELLO: {
        ReceiveHelloPacket(singletonServer, inputStream, fromSocketAddress, playerID);
        break;
    }

    case ClientMessageType::INPUT: {
        ReceiveInputPacket(singletonServer, inputStream, playerID);
        break;
    }

    default: {
        WLOG("Unknown packet received from socket address %s", fromSocketAddress.GetName());
        break;
    }
    }
    assert(playerID != INVALID_PLAYER_ID);

    std::shared_ptr<ClientProxy> clientProxy = singletonServer.GetClientProxyFromPlayerID(playerID);
    clientProxy->UpdateLastPacketTime();
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveHelloPacket(SingletonServerComponent& singletonServer, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress, PlayerID& playerID)
{
    playerID = singletonServer.GetPlayerID(fromSocketAddress);
    if (playerID == INVALID_PLAYER_ID) {
        std::string name;
        inputStream.Read(name);
        ILOG("Hello packet received from new player %s", name.c_str());

        playerID = singletonServer.AddPlayer(fromSocketAddress, name.c_str());
        if (playerID != INVALID_PLAYER_ID) {

            Event newEvent;
            newEvent.eventType = EventType::PLAYER_ADDED;
            newEvent.server.playerID = playerID;
            PushEvent(newEvent);

            ILOG("New player %s %u has joined the game", name.c_str(), playerID);
        }
    } else {
        std::shared_ptr<ClientProxy> clientProxy = singletonServer.GetClientProxyFromPlayerID(playerID);
        ILOG("Hello packet received from existing player %s", clientProxy->GetName());
    }

    SendWelcomePacket(singletonServer, playerID, fromSocketAddress);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveInputPacket(SingletonServerComponent& singletonServer, InputMemoryStream& inputStream, PlayerID& playerID) const
{
    inputStream.Read(playerID);
    std::shared_ptr<ClientProxy> clientProxy = singletonServer.GetClientProxyFromPlayerID(playerID);
    const bool isValid = clientProxy->m_deliveryManager->ReadState(inputStream);
    if (clientProxy == nullptr || !isValid) {
        ILOG("Input packet received from unknown player");
        playerID = INVALID_PLAYER_ID;
        return;
    }

    ILOG("Input packet received from player %s", clientProxy->GetName());

    U32 moveCount = 0;
    inputStream.Read(moveCount, GetRequiredBits<MAX_MOVES_PER_PACKET>::value);
    if (moveCount > 0) {
        Move move;
        while (moveCount > 0) {
            move.Read(inputStream);
            clientProxy->AddMove(move);
            --moveCount;
        }

        clientProxy->m_isLastMoveTimestampDirty = true;
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnConnectionReset(const SocketAddress& /*fromSocketAddress*/) const
{
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnDisconnect() const
{
}
}
