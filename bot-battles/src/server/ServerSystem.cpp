#include "ServerSystem.h"

#include "ClientProxy.h"
#include "ComponentMemberTypes.h"
#include "DeliveryManagerServer.h"
#include "EntityManager.h"
#include "GameServer.h"
#include "LinkingContext.h"
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

    case EventType::NETWORK_ENTITY_ADDED: {
        OnNetworkEntityAdded(event.networking.networkID);
        break;
    }

    case EventType::NETWORK_ENTITY_REMOVED: {
        OnNetworkEntityRemoved(event.networking.networkID);
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
    serverComponent.m_socketAddress = SocketAddress::CreateIPv4(INADDR_ANY, static_cast<U16>(atoi(serverComponent.m_port.c_str())));
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
void ServerSystem::ReceiveIncomingPackets(ServerComponent& serverComponent)
{
    InputMemoryStream packet;
    U32 byteCapacity = packet.GetByteCapacity();
    SocketAddress fromSocketAddress;

    U32 receivedPacketCount = 0;

    while (receivedPacketCount < MAX_PACKETS_PER_FRAME) {
        I32 readByteCount = serverComponent.m_socket->ReceiveFrom(packet.GetPtr(), byteCapacity, fromSocketAddress);
        if (readByteCount > 0) {
            packet.SetCapacity(readByteCount);
            packet.ResetHead();
            ReceivePacket(serverComponent, packet, fromSocketAddress);
            ++receivedPacketCount;
        } else if (readByteCount == -WSAECONNRESET) {
            ConnectionReset(serverComponent, fromSocketAddress);
        } else if (readByteCount == 0 || -WSAEWOULDBLOCK) {
            // TODO: graceful disconnection if readByteCount == 0?
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendOutgoingPackets(ServerComponent& serverComponent)
{
    std::unordered_map<PlayerID, std::weak_ptr<ClientProxy>> playerIDToClientProxyDisconnections;

    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        PlayerID playerID = pair.first;
        std::shared_ptr<ClientProxy> clientProxy = pair.second;

        F32 timeout = Time::GetInstance().GetTime() - clientProxy->GetLastPacketTime();
        if (timeout >= DISCONNECT_TIMEOUT) {
            playerIDToClientProxyDisconnections.insert(std::make_pair(pair.first, std::weak_ptr(pair.second)));
            continue;
        }

        const bool isConnected = clientProxy->m_deliveryManager.GetNextExpectedSequenceNumber() > 0;
        if (isConnected) {
            clientProxy->m_deliveryManager.ProcessTimedOutPackets();
            SendStatePacket(serverComponent, playerID, clientProxy);
        } else {
            SendWelcomePacket(serverComponent, playerID, clientProxy);
        }
    }

    for (const auto& pair : playerIDToClientProxyDisconnections) {
        PlayerID playerID = pair.first;
        Entity entity = serverComponent.GetEntity(playerID);
        Disconnect(serverComponent, playerID, entity);
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

    if (playerID < INVALID_PLAYER_ID && type < ClientMessageType::COUNT) {
        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        clientProxy.lock()->UpdateLastPacketTime();
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveHelloPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, const SocketAddress& fromSocketAddress, PlayerID& playerID)
{
    playerID = serverComponent.GetPlayerID(fromSocketAddress);
    if (playerID >= INVALID_PLAYER_ID) {
        std::string name;
        inputStream.Read(name);
        ILOG("Hello packet received from new player %s", name.c_str());

        playerID = serverComponent.AddPlayer(fromSocketAddress, name.c_str());
        if (playerID < INVALID_PLAYER_ID) {
            std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
            const std::unordered_map<NetworkID, Entity>& networkIDToEntity = g_gameServer->GetLinkingContext().GetNetworkIDToEntityMap();
            for (const auto& pair : networkIDToEntity) {
                clientProxy.lock()->m_replicationManager->AddCommand(pair.first, static_cast<U32>(ComponentMemberType::ALL));
            }

            Event newEvent;
            newEvent.eventType = EventType::PLAYER_ADDED;
            newEvent.networking.playerID = playerID;
            NotifyEvent(newEvent);

            ILOG("New player %s %u has joined the game", name.c_str(), playerID);
        }
    } else {
        std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
        ILOG("Hello packet received from existing player %s", clientProxy.lock()->GetName());
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::ReceiveInputPacket(ServerComponent& serverComponent, InputMemoryStream& inputStream, PlayerID& playerID) const
{
    inputStream.Read(playerID);
    std::weak_ptr<ClientProxy> clientProxy = serverComponent.GetClientProxy(playerID);
    if (clientProxy.expired()) {
        ILOG("Input packet received from unknown player");
        playerID = INVALID_PLAYER_ID;
        return;
    }

    const bool isValid = clientProxy.lock()->m_deliveryManager.ReadState(inputStream);
    if (!isValid) {
        ILOG("Input packet received but skipped because it is not valid");
        return;
    }

    ILOG("Input packet received from player %u %s", playerID, clientProxy.lock()->GetName());

    inputStream.Read(clientProxy.lock()->m_timestamp);

    // TODO: should we have individual frames for Input packets or just send a single frame for the last Input sent?

    bool hasInputs = false;
    inputStream.Read(hasInputs);
    if (hasInputs) {
        U32 inputCount = 0;
        inputStream.Read(inputCount);
        while (inputCount > 0) {
            Input input;
            input.Read(inputStream);
            if (input.GetFrame() > clientProxy.lock()->m_lastAckdFrame) { // TODO: be careful if new frame is 15 and last frame is 13 and frame 14 contains a shoot for example
                clientProxy.lock()->m_inputBuffer.Add(input);
                clientProxy.lock()->m_lastAckdFrame = input.GetFrame();
            }
            --inputCount;
        }
    }

    ILOG("Server received ackd frame %u", clientProxy.lock()->m_lastAckdFrame);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendWelcomePacket(const ServerComponent& serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const
{
    OutputMemoryStream welcomePacket;
    welcomePacket.Write(ServerMessageType::WELCOME);
    const bool isSuccessful = playerID < INVALID_PLAYER_ID;
    welcomePacket.Write(isSuccessful);
    if (isSuccessful) {
        welcomePacket.Write(playerID);
    }

    ILOG("Sending welcome packet to player %u...", playerID);

    const bool result = SendPacket(serverComponent, welcomePacket, clientProxy->GetSocketAddress());
    if (result) {
        ILOG("Welcome packet successfully sent to player %u", playerID);
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::SendStatePacket(const ServerComponent& serverComponent, PlayerID playerID, std::shared_ptr<ClientProxy> clientProxy) const
{
    const bool hasEntity = serverComponent.GetEntity(playerID) < INVALID_ENTITY;
    if (!hasEntity) {
        ILOG("State packet not sent because entity is not created");
        return;
    }

    OutputMemoryStream statePacket;
    statePacket.Write(ServerMessageType::STATE);

    Delivery& delivery = clientProxy->m_deliveryManager.WriteState(statePacket);
    delivery.m_replicationResultManager = std::make_shared<ReplicationResultManager>(std::weak_ptr<ReplicationManagerServer>(clientProxy->m_replicationManager));

    statePacket.Write(clientProxy->m_timestamp);

    ILOG("SERVER SENT ACKD FRAME %u", clientProxy->m_lastAckdFrame);
    statePacket.Write(clientProxy->m_lastAckdFrame);

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
void ServerSystem::ConnectionReset(ServerComponent& serverComponent, const SocketAddress& socketAddress)
{
    PlayerID playerID = serverComponent.GetPlayerID(socketAddress);
    Entity entity = serverComponent.GetEntity(playerID);
    Disconnect(serverComponent, playerID, entity);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::Disconnect(ServerComponent& serverComponent, PlayerID playerID, Entity entity)
{
    serverComponent.RemovePlayer(playerID);

    Event newEvent;
    newEvent.eventType = EventType::PLAYER_REMOVED;
    newEvent.networking.entity = entity;
    NotifyEvent(newEvent);
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnNetworkEntityAdded(NetworkID networkID) const
{
    assert(networkID < INVALID_NETWORK_ID);

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        pair.second->m_replicationManager->AddCommand(networkID, static_cast<U32>(ComponentMemberType::ALL));
    }
}

//----------------------------------------------------------------------------------------------------
void ServerSystem::OnNetworkEntityRemoved(NetworkID networkID) const
{
    assert(networkID < INVALID_NETWORK_ID);

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
    if (networkID >= INVALID_NETWORK_ID) {
        return;
    }

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    const std::unordered_map<PlayerID, std::shared_ptr<ClientProxy>>& playerIDToClientProxy = serverComponent.GetPlayerIDToClientProxyMap();
    for (const auto& pair : playerIDToClientProxy) {
        pair.second->m_replicationManager->AddDirtyState(networkID, dirtyState);
    }
}
}
