#include "ScoreboardStateClient.h"

#include "ComponentManager.h"
#include "ConfigClient.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameClient.h"
#include "ScoreboardComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
const char* ScoreboardStateClient::GetName() const
{
    return "Scoreboard";
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateClient::Enter() const
{
    ILOG("Entering %s...", GetName());

    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    scoreboardComponent.m_phase = ScoreboardComponent::ScoreboardPhase::RESULTS;
    scoreboardComponent.m_timer.Start();

    Entity background = g_gameClient->GetEntityManager().AddEntity();
    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
    std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().AddComponent<TransformComponent>(background);
    transformComponent.lock()->m_position = { static_cast<F32>(windowComponent.m_resolution.x) / 2.0f, static_cast<F32>(windowComponent.m_resolution.y) / 2.0f, static_cast<F32>(LayerType::BACKGROUND) };
    std::weak_ptr<SpriteResource> spriteResource = g_gameClient->GetResourceManager().AddResource<SpriteResource>("scoreboardBackground.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().AddComponent<SpriteComponent>(background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateClient::Update() const
{
    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    F32 time = static_cast<F32>(scoreboardComponent.m_timer.ReadSec());
    if (time >= scoreboardComponent.m_mainMenuTimeout) {
        g_gameClient->GetFSM().ChangeState(g_gameClient->GetConfig().m_offlineSceneName.c_str());
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateClient::RenderGui() const
{
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoSavedSettings;

    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
    ImVec2 position = ImVec2(static_cast<F32>(windowComponent.m_resolution.x) / 2.0f, static_cast<F32>(windowComponent.m_resolution.y) / 2.0f);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImVec2 size = ImVec2(static_cast<F32>(windowComponent.m_resolution.y) / 2.0f, static_cast<F32>(windowComponent.m_resolution.x) / 2.0f);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    if (ImGui::Begin(GetName(), nullptr, windowFlags)) {
        ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
        switch (scoreboardComponent.m_phase) {
        case ScoreboardComponent::ScoreboardPhase::RESULTS: {
            RenderResultsGui(scoreboardComponent);
            break;
        }

        case ScoreboardComponent::ScoreboardPhase::RESTART: {
            RenderRestartGui(scoreboardComponent);
            break;
        }

        default: {
            break;
        }
        }

        ImGui::End();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ScoreboardStateClient::Exit() const
{
    ILOG("Exiting %s...", GetName());

    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    scoreboardComponent.m_phase = ScoreboardComponent::ScoreboardPhase::NONE;

    g_gameClient->GetEntityManager().ClearEntities();

    return true;
}

//----------------------------------------------------------------------------------------------------
void ScoreboardStateClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::REWELCOME_RECEIVED: {
        OnReWelcomeReceived();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ScoreboardStateClient::RenderResultsGui(ScoreboardComponent& scoreboardComponent) const
{
    F32 time = static_cast<F32>(scoreboardComponent.m_timer.ReadSec());
    F32 timeLeft = scoreboardComponent.m_mainMenuTimeout - time;
    ImGui::Text("%.0f", timeLeft);

    ImGui::Text("Player x wins");

    const char* playAgain = "Play again";
    const char* mainMenu = "Main menu";
    ImVec2 playAgainTextSize = ImGui::CalcTextSize(playAgain);
    ImVec2 mainMenuTextSize = ImGui::CalcTextSize(mainMenu);
    ImVec2 framePadding = ImGui::GetStyle().FramePadding;
    ImVec2 playAgainButtonSize = ImVec2(playAgainTextSize.x + framePadding.x * 2.0f, playAgainTextSize.y + framePadding.y * 2.0f);
    ImVec2 mainMenuButtonSize = ImVec2(mainMenuTextSize.x + framePadding.x * 2.0f, mainMenuTextSize.y + framePadding.y * 2.0f);
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImVec2 itemSpacing = ImGui::GetStyle().ItemSpacing;
    ImGui::SetCursorPosX(contentRegionMax.x - playAgainButtonSize.x - mainMenuButtonSize.x - itemSpacing.x);
    ImGui::SetCursorPosY(contentRegionMax.y - playAgainButtonSize.y);
    if (ImGui::Button(playAgain)) {
        scoreboardComponent.m_phase = ScoreboardComponent::ScoreboardPhase::RESTART;

        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        clientComponent.m_sendReHelloPacket = true;
    }
    ImGui::SetCursorPosX(contentRegionMax.x - mainMenuButtonSize.x);
    ImGui::SetCursorPosY(contentRegionMax.y - mainMenuButtonSize.y);
    if (ImGui::Button(mainMenu)) {
        g_gameClient->GetFSM().ChangeState(g_gameClient->GetConfig().m_offlineSceneName.c_str());

        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        clientComponent.m_sendByePacket = true;
    }
}

//----------------------------------------------------------------------------------------------------
void ScoreboardStateClient::RenderRestartGui(ScoreboardComponent& scoreboardComponent) const
{
    F32 time = static_cast<F32>(scoreboardComponent.m_timer.ReadSec());
    if (time >= 3.0f) {
        scoreboardComponent.m_timer.Start();
    }

    if (time >= 2.0f) {
        ImGui::Text("Waiting...");
    } else if (time >= 1.0f) {
        ImGui::Text("Waiting..");
    } else if (time >= 0.0f) {
        ImGui::Text("Waiting.");
    }

    const char* cancel = "Cancel";
    ImVec2 textSize = ImGui::CalcTextSize(cancel);
    ImVec2 framePadding = ImGui::GetStyle().FramePadding;
    ImVec2 buttonSize = ImVec2(textSize.x + framePadding.x * 2.0f, textSize.y + framePadding.y * 2.0f);
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImGui::SetCursorPosX(contentRegionMax.x - buttonSize.x);
    ImGui::SetCursorPosY(contentRegionMax.y - buttonSize.y);
    if (ImGui::Button(cancel)) {
        scoreboardComponent.m_phase = ScoreboardComponent::ScoreboardPhase::RESULTS;
    }
}

//----------------------------------------------------------------------------------------------------
void ScoreboardStateClient::OnReWelcomeReceived() const
{
    g_gameClient->GetFSM().ChangeState(g_gameClient->GetConfig().m_onlineSceneName.c_str());
}
}
