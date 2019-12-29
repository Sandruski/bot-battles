#include "ClientComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ClientComponent::ClientComponent()
    : m_socket(nullptr)
    , m_socketAddress(nullptr)
    , m_name()
    , m_ip()
    , m_port()
    , m_playerID(INVALID_PLAYER_ID)
    , m_lastDeliveryTimestamp(0.0f)
{
}

void ClientComponent::LoadFromConfig(const rapidjson::Value& value)
{
    assert(value.HasMember("name"));
    assert(value["name"].IsString());
    m_name = value["name"].GetString();

    assert(value.HasMember("ip"));
    assert(value["ip"].IsString());
    m_ip = value["ip"].GetString();

    assert(value.HasMember("port"));
    assert(value["port"].IsString());
    m_port = value["port"].GetString();
}

//----------------------------------------------------------------------------------------------------
bool ClientComponent::IsConnected() const
{
    return m_playerID != INVALID_PLAYER_ID;
}

//----------------------------------------------------------------------------------------------------
F32 ClientComponent::GetNextHelloTime() const
{
    return m_lastDeliveryTimestamp + TIME_BETWEEN_HELLO;
}

//----------------------------------------------------------------------------------------------------
F32 ClientComponent::GetNextInputTime() const
{
    return m_lastDeliveryTimestamp + TIME_BETWEEN_INPUT;
}
}
