#include "PlayStateServer.h"

#include "ComponentManager.h"
#include "ConfigServer.h"
#include "EventComponent.h"
#include "FSM.h"
#include "GameServer.h"
#include "HealthComponent.h"
#include "ScoreboardComponent.h"
#include "ServerComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string PlayStateServer::GetName() const
{
    return "Play";
}

//----------------------------------------------------------------------------------------------------
bool PlayStateServer::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    serverComponent.lock()->m_durationTimer.Start();

    Event newEvent;
    newEvent.eventType = EventType::SEND_PLAY;
    g_gameServer->GetFSM().NotifyEvent(newEvent);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool PlayStateServer::Update() const
{
    std::weak_ptr<EventComponent> eventComponent = g_game->GetEventComponent();
    if (eventComponent.lock()->m_keyboard.at(SDL_SCANCODE_LSHIFT) == EventComponent::KeyState::REPEAT
        && eventComponent.lock()->m_keyboard.at(SDL_SCANCODE_W) == EventComponent::KeyState::DOWN) {
        ChangeToScoreboard();
    }

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    if (serverComponent.lock()->m_duration <= 0) {
        return true;
    }

    F32 durationCurrentTime = static_cast<F32>(serverComponent.lock()->m_durationTimer.ReadSec());
    if (durationCurrentTime >= static_cast<F32>(serverComponent.lock()->m_duration)) {
        ChangeToScoreboard();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool PlayStateServer::RenderGui() const
{
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoScrollWithMouse;
    windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
    windowFlags |= ImGuiWindowFlags_NoBackground;
    windowFlags |= ImGuiWindowFlags_NoSavedSettings;
    windowFlags |= ImGuiWindowFlags_NoFocusOnAppearing;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    windowFlags |= ImGuiWindowFlags_NoDecoration;
    windowFlags |= ImGuiWindowFlags_NoInputs;

    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    glm::vec2 proportion = windowComponent.lock()->GetProportion();

    ImVec2 windowPosition = ImVec2(windowComponent.lock()->m_baseResolution.x / 2.0f, 20.0f);
    windowPosition.x *= proportion.x;
    windowPosition.y *= proportion.y;
    ImGui::SetNextWindowPos(windowPosition, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    if (ImGui::Begin("Time", nullptr, windowFlags)) {
        glm::vec4 color = White;
        ImVec4 colorText = ImVec4(color.r, color.g, color.b, color.a);

        std::string text;
        std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
        if (serverComponent.lock()->m_duration <= 0) {
            text = "-";
        } else {
            F32 timeDiff = static_cast<F32>(serverComponent.lock()->m_duration) - static_cast<F32>(serverComponent.lock()->m_durationTimer.ReadSec());
            text = std::to_string(static_cast<I32>(std::ceil(timeDiff)));
        }

        ImGui::TextColored(colorText, text.c_str());

        ImGui::End();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool PlayStateServer::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
void PlayStateServer::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::HEALTH_HURT: {
        OnHealthHurt();
        break;
    }

    case EventType::PLAYER_REMOVED: {
        ChangeToScoreboard();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void PlayStateServer::OnHealthHurt() const
{
    U32 aliveCount = 0;
    Entity winnerEntity = INVALID_ENTITY;

    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    std::vector<std::pair<Entity, std::weak_ptr<HealthComponent>>> healthComponents = g_gameServer->GetComponentManager().GetComponents<HealthComponent>();
    for (const auto& pair : healthComponents) {
        Entity ownerEntity = pair.first;
        PlayerID playerID = serverComponent.lock()->GetPlayerID(ownerEntity);
        if (playerID >= INVALID_PLAYER_ID) {
            continue;
        }

        std::weak_ptr<HealthComponent> healthComponent = pair.second;
        if (healthComponent.expired()) {
            continue;
        }

        const bool isAlive = healthComponent.lock()->m_currentHP > 0;
        if (isAlive) {
            ++aliveCount;
            winnerEntity = ownerEntity;
        }
    }

    // V X
    if (aliveCount == 0 || aliveCount == 1) {
        std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameServer->GetScoreboardComponent();
        scoreboardComponent.lock()->m_winnerPlayerID = serverComponent.lock()->GetPlayerID(winnerEntity);

        ChangeToScoreboard();
    }
}

//----------------------------------------------------------------------------------------------------
void PlayStateServer::ChangeToScoreboard() const
{
    g_gameServer->GetFSM().ChangeState("Scoreboard");
}
}
