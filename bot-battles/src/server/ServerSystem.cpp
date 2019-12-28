#include "ServerSystem.h"

#include "ClientProxy.h"
#include "ComponentMemberTypes.h"
#include "DeliveryManagerServer.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "LinkingContext.h"
#include "MemoryStream.h"
#include "MessageTypes.h"
#include "ReplicationManagerServer.h"
#include "ReplicationResultManager.h"
#include "ServerComponent.h"
#include "UDPSocket.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::ENTITY_ADDED: {
        OnEntityAdded(event.entity.entity);
        break;
    }

    case EventType::ENTITY_REMOVED: {
        OnEntityRemoved(event.entity.entity);
        break;
    }

    case EventType::COMPONENT_MEMBER_CHANGED: {
        OnComponentMemberChanged(event.component.dirtyState, event.component.entity);
        break;
    }

    default: {
        break;
    }
    }
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

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    serverComponent.m_socketAddress = SocketAddress::CreateIPv4(INADDR_ANY, static_cast<U16>(atoi("9999")));
    assert(serverComponent.m_socketAddress != nullptr);
    serverComponent.m_socket = UDPSocket::CreateIPv4();
    assert(serverComponent.m_socket != nullptr);
    serverComponent.m_socket->SetNonBlockingMode(true);
    serverComponent.m_socket->Bind(*serverComponent.m_socketAddress);

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
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();

    ReceiveIncomingPackets(serverComponent);
    // TODO: ProcessQueuedPackets()
    SendOutgoingPackets(serverComponent);

    return true;
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendOutgoingPackets(ServerComponent& serverComponent)
{
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        std::shared_ptr<ClientProxy> clientProxy = pair.second;
        F32 timeout = Time::GetInstance().GetTime() - clientProxy->GetLastPacketTime();
        if (timeout >= DISCONNECT_TIMEOUT) {
            PlayerID playerID = pair.first;
            Entity entity = serverComponent.GetEntity(clientProxy->GetSocketAddress());
            DisconnectClient(serverComponent, playerID, entity);
        }

        clientProxy->m_deliveryManager->ProcessTimedOutPackets();

        //if (clientProxy->m_isLastMoveTimestampDirty) {
        SendStatePacket(serverComponent, clientProxy);
        //}
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendWelcomePacket(const ServerComponent& serverComponent, PlayerID playerID, const SocketAddress& toSocketAddress) const
{
    OutputMemoryStream welcomePacket;
    welcomePacket.Write(ServerMessageType::WELCOME);
    const bool isSuccessful = playerID != INVALID_PLAYER_ID;
    welcomePacket.Write(isSuccessful);
    if (isSuccessful) {
        welcomePacket.Write(playerID);
    }

    ILOG("Sending welcome packet to player %u...", playerID);

    const bool result = SendPacket(serverComponent, welcomePacket, toSocketAddress);
    if (result) {
        ILOG("Welcome packet successfully sent to player %u", playerID);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendStatePacket(const ServerComponent& serverComponent, std::shared_ptr<ClientProxy> clientProxy) const
{
    OutputMemoryStream statePacket;
    statePacket.Write(ServerMessageType::STATE);
    Delivery& delivery = clientProxy->m_deliveryManager->WriteState(statePacket);
    delivery.m_replicationResultManager = std::make_shared<ReplicationResultManager>(clientProxy->m_replicationManager);
    clientProxy->m_replicationManager->Write(statePacket, *delivery.m_replicationResultManager);

    const char* name = clientProxy->GetName();
    ILOG("Sending state packet to player %s...", name);

    const bool result = SendPacket(serverComponent, statePacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("State packet successfully sent to player %s", name);
    }
}

//----------------------------------------------------------------------------------------------------
bool ServerSystem::SendPacket(const ServerComponent& serverComponent, const OutputMemoryStream& outputStream, const SocketAddress& toSocketAddress) const
{
    return serverComponent.m_socket->SendTo(outputStream.GetPtr(), outputStream.GetByteLength(), toSocketAddress);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveIncomingPackets(ServerComponent& serverComponent)
{
    InputMemoryStream packet;
    SocketAddress fromSocketAddress;

    U32 receivedPacketCount = 0;

    while (receivedPacketCount < MAX_PACKETS_PER_FRAME) {
        I32 readByteCount = serverComponent.m_socket->ReceiveFrom(packet.GetPtr(), packet.GetByteCapacity(), fromSocketAddress);
        if (readByteCount > 0) {
            packet.SetCapacity(readByteCount);
            packet.ResetHead();
            ReceivePacket(serverComponent, packet, fromSocketAddress);

            ++receivedPacketCount;
        } else if (readByteCount == -WSAECONNRESET) {
            ClientConnectionReset(serverComponent, fromSocketAddress);
        } else if (readByteCount == 0 || -WSAEWOULDBLOCK) {
            // TODO: graceful disconnection if readByteCount == 0?
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceivePacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress)
{
    ClientMessageType type;
    inputStream.Read(type);
    PlayerID playerID = INVALID_PLAYER_ID;
    switch (type) {
    case ClientMessageType::HELLO: {
        ReceiveHelloPacket(serverComponent, inputStream, fromSocketAddress, playerID);
        break;
    }

    case ClientMessageType::INPUT: {
        ReceiveInputPacket(serverComponent, inputStream, playerID);
        break;
    }

    default: {
        WLOG("Unknown packet received from socket address %s", fromSocketAddress.GetName());
        break;
    }
    }
    assert(playerID != INVALID_PLAYER_ID);

    std::shared_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxyFromPlayerID(playerID);
    clientProxy->UpdateLastPacketTime();
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveHelloPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress, PlayerID& playerID)
{
    playerID = serverComponent.GetPlayerID(fromSocketAddress);
    if (playerID == INVALID_PLAYER_ID) {
        std::string name;
        inputStream.Read(name);
        ILOG("Hello packet received from new player %s", name.c_str());

        playerID = serverComponent.AddPlayer(fromSocketAddress, name.c_str());
        if (playerID != INVALID_PLAYER_ID) {

            std::shared_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxyFromPlayerID(playerID);
            const std::unordered_map<NetworkID, Entity>& networkIDToEntity = g_gameServer->GetLinkingContext().GetNetworkIDToEntityMap();
            for (const auto& pair : networkIDToEntity) {
                clientProxy->m_replicationManager->AddCommand(pair.first, static_cast<U32>(ComponentMemberType::ALL));
            }

            Event newEvent;
            newEvent.eventType = EventType::PLAYER_ADDED;
            newEvent.server.playerID = playerID;
            PushEvent(newEvent);

            ILOG("New player %s %u has joined the game", name.c_str(), playerID);
        }
    } else {
        std::shared_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxyFromPlayerID(playerID);
        ILOG("Hello packet received from existing player %s", clientProxy->GetName());
    }

    SendWelcomePacket(serverComponent, playerID, fromSocketAddress);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveInputPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, PlayerID& playerID) const
{
    inputStream.Read(playerID);
    std::shared_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxyFromPlayerID(playerID);
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
void ServerSystem::ClientConnectionReset(ServerComponent& serverComponent, const SocketAddress& socketAddress)
{
    PlayerID playerID = serverComponent.GetPlayerID(socketAddress);
    Entity entity = serverComponent.GetEntity(socketAddress);
    DisconnectClient(serverComponent, playerID, entity);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::DisconnectClient(ServerComponent& serverComponent, PlayerID playerID, Entity entity)
{
    serverComponent.RemovePlayer(playerID);

    Event newEvent;
    newEvent.eventType = EventType::PLAYER_REMOVED;
    newEvent.server.entity = entity;
    PushEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnEntityAdded(Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    NetworkID networkID = g_gameServer->GetLinkingContext().GetNetworkID(entity);
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        pair.second->m_replicationManager->AddCommand(networkID, static_cast<U32>(ComponentMemberType::ALL));
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnEntityRemoved(Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    NetworkID networkID = g_gameServer->GetLinkingContext().GetNetworkID(entity);
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        pair.second->m_replicationManager->SetRemove(networkID);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnComponentMemberChanged(U32 dirtyState, Entity entity) const
{
    assert(entity < INVALID_ENTITY);

    NetworkID networkID = g_gameServer->GetLinkingContext().GetNetworkID(entity);
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        pair.second->m_replicationManager->AddDirtyState(networkID, dirtyState);
    }
}
}
