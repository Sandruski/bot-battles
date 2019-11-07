#include "ClientSystem.h"

#include "Game.h"
#include "MemoryStream.h"
#include "MessageTypes.h"
#include "Move.h"
#include "MoveList.h"
#include "SingletonClientComponent.h"
#include "SocketAddress.h"
#include "UDPSocket.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ClientSystem::ClientSystem()
{
}

//----------------------------------------------------------------------------------------------------
ClientSystem::~ClientSystem()
{
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::StartUp()
{
    WORD winsockVersion = MAKEWORD(2, 2);
    WSADATA winsockData;
    int iResult = WSAStartup(winsockVersion, &winsockData);
    if (iResult == SOCKET_ERROR) {
        NETLOG("WSAStartup");
        return false;
    }

    std::shared_ptr<SingletonClientComponent> client = g_game->GetSingletonClientComponent();
    client->m_socketAddress = SocketAddress::CreateIPv4("127.0.0.1", "9999");
    client->m_socket = UDPSocket::CreateIPv4();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::Update()
{
    std::shared_ptr<SingletonClientComponent> client = g_game->GetSingletonClientComponent();
    switch (client->m_state) {
    case SingletonClientComponent::ClientState::SAY_HELLO: {
        UpdateSayHello(*client);
        break;
    }

    case SingletonClientComponent::ClientState::SEND_INPUT: {
        UpdateSendInput(*client);
        break;
    }

    default: {
        break;
    }
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::ShutDown()
{
    return true;
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::UpdateSayHello(SingletonClientComponent& client) const
{
    float time = Time::GetInstance().GetTime();
    float nextTime = client.m_lastTime + client.m_timeBetweenSayHello;
    if (time >= nextTime) {
        SendHelloPacket(client);

        client.m_lastTime = time;
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::UpdateSendInput(SingletonClientComponent& client) const
{
    float time = Time::GetInstance().GetTime();
    float nextTime = client.m_lastTime + client.m_timeBetweenSendInput;
    if (time >= nextTime) {
        SendInputPacket(client);

        client.m_lastTime = time;
    }
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::SendHelloPacket(const SingletonClientComponent& client) const
{
    OutputMemoryStream helloPacket;
    helloPacket.Write(ClientMessageType::HELLO);
    helloPacket.Write(client.m_name);

    SendPacket(client, helloPacket);
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::SendInputPacket(const SingletonClientComponent& client) const
{
    MoveList moves; // TODO: input system
    if (!moves.HasMoves()) {
        return;
    }

    OutputMemoryStream inputPacket;
    inputPacket.Write(ClientMessageType::INPUT);

    U32 moveCount = moves.GetMoveCount();
    U32 startIndex = moveCount > 3 ? moveCount - 3 - 1 : 0;
    inputPacket.Write(moveCount - startIndex, 2);
    for (U32 i = startIndex; i < moveCount; ++i) {
        moves.GetMove(i).Write(inputPacket);
    }

    SendPacket(client, inputPacket);

    moves.Clear();
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveWelcomePacket(SingletonClientComponent& client, InputMemoryStream& stream) const
{
    if (client.m_state != SingletonClientComponent::ClientState::SAY_HELLO) {
        return;
    }

    stream.Read(client.m_playerID);
    ILOG("Player %s has been welcomed as %u", client.m_name.c_str(), client.m_playerID);

    client.m_state = SingletonClientComponent::ClientState::SEND_INPUT;
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceiveStatePacket(SingletonClientComponent& /*client*/, InputMemoryStream& /*stream*/) const
{
}

//----------------------------------------------------------------------------------------------------
bool ClientSystem::SendPacket(const SingletonClientComponent& client, const OutputMemoryStream& stream) const
{
    client.m_socket->SendTo(stream.GetPtr(), stream.GetByteLength(), *client.m_socketAddress);

    return true;
}

//----------------------------------------------------------------------------------------------------
void ClientSystem::ReceivePacket(SingletonClientComponent& client, InputMemoryStream& stream) const
{
    ServerMessageType type;
    stream.Read(type);
    switch (type) {
    case ServerMessageType::WELCOME: {
        ReceiveWelcomePacket(client, stream);
        break;
    }

    case ServerMessageType::STATE: {
        ReceiveStatePacket(client, stream);
        break;
    }

    default: {
        break;
    }
    }
}
}
