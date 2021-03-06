#include "ClientComponent.h"

#include "FileSystem.h"
#include "GameClient.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ClientComponent::ClientComponent()
    : m_replicationManager()
    , m_deliveryManager()
    , m_UDPSocket()
    , m_TCPSocket()
    , m_socketAddress()
    , m_ip()
    , m_port()
    , m_map()
    , m_duration(0)
    , m_durationTime(0.0f)
    , m_name()
    , m_bot()
    , m_damageInflicted(0)
    , m_damageReceived(0)
    , m_playerID(INVALID_PLAYER_ID)
    , m_entity(INVALID_ENTITY)
    , m_incomingPacketsTimer()
    , m_outgoingPacketsTimer()
    , m_incomingPacketsTimeout(1.0f / 60.0f)
    , m_outgoingPacketsTimeout(1.0f / 60.0f)
    , m_lastPacketTime(0.0f)
    , m_lastAckdFrame(0)
    , m_interpolationFromFrame(0)
    , m_interpolationToFrame(0)
    , m_interpolationPercentage(0.0f)
    , m_entityInterpolationPeriod(1.0f / 20.0f)
    , m_frameBuffer()
    , m_inputBuffer()
    , m_isLastMovementInputPending(false)
    , m_isLastWeaponInputPending(false)
    , m_isLastHealthInputPending(false)
    , m_secondsBetweenConnect(5.0f)
    , m_connectTimer()
    , m_RTT(0.0f)
    , m_disconnectTimeout(15.0f) // 5.0f
    , m_maxPacketsPerFrame(10) // same as server
    , m_maxInputsPerPaquet(30) // same as server
    , m_isClientPredictionServerReconciliation(true)
    , m_isClientPrediction(true)
    , m_isServerReconciliation(true)
    , m_isEntityInterpolation(true)
{
}

//----------------------------------------------------------------------------------------------------
void ClientComponent::LoadFromConfig(const rapidjson::Value& value)
{
    if (value.HasMember("ip") && value["ip"].IsString()) {
        m_ip = value["ip"].GetString();
    }

    if (value.HasMember("port") && value["port"].IsString()) {
        m_port = value["port"].GetString();
    }

    if (value.HasMember("name") && value["name"].IsString()) {
        m_name = value["name"].GetString();
    }

    if (value.HasMember("bot") && value["bot"].IsString()) {
        m_bot = value["bot"].GetString();
        std::vector<std::string> entries = g_gameClient->GetFileSystem().GetFilesFromDirectory(BOTS_SCRIPTS_DIR, SCRIPTS_EXTENSION, false);
        bool hasScript = false;
        for (const auto& entry : entries) {
            std::string name = g_gameClient->GetFileSystem().GetName(entry);
            if (name == m_bot) {
                hasScript = true;
                break;
            }
        }
        if (!hasScript) {
            if (!entries.empty()) {
                std::string name = g_gameClient->GetFileSystem().GetName(entries.front());
                m_bot = name;
            } else {
                m_bot = "";
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------
bool ClientComponent::IsConnected() const
{
    return m_playerID < INVALID_PLAYER_ID;
}

//----------------------------------------------------------------------------------------------------
bool ClientComponent::IsLocalEntity(Entity entity) const
{
    return entity == m_entity;
}

//----------------------------------------------------------------------------------------------------
bool ClientComponent::IsLocalPlayer(PlayerID playerID) const
{
    return playerID == m_playerID;
}

//----------------------------------------------------------------------------------------------------
F32 ClientComponent::GetRatio() const
{
    return static_cast<F32>(m_damageInflicted) / static_cast<F32>(m_damageReceived);
}

//----------------------------------------------------------------------------------------------------
void ClientComponent::Reset()
{
    m_deliveryManager.Reset();
    m_lastPacketTime = 0.0f;
    m_lastAckdFrame = 0;
    m_interpolationFromFrame = 0;
    m_interpolationToFrame = 0;
    m_interpolationPercentage = 0.0f;
    m_frameBuffer.Clear();
    m_inputBuffer.Clear();
    m_isLastMovementInputPending = false;
    m_isLastWeaponInputPending = false;
    m_isLastHealthInputPending = false;
    m_RTT = 0.0f;
}
}
