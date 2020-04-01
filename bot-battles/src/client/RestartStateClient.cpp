#include "RestartStateClient.h"

#include "ConfigClient.h"
#include "GameClient.h"
#include "ScoreboardComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string RestartStateClient::GetName() const
{
    return "Restart";
}

//----------------------------------------------------------------------------------------------------
bool RestartStateClient::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    scoreboardComponent.m_guiTimer.Start();

    Event newEvent;
    newEvent.eventType = EventType::SEND_REHELLO;
    g_gameClient->GetFSM().NotifyEvent(newEvent);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RestartStateClient::Update() const
{
    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    F32 time = static_cast<F32>(scoreboardComponent.m_mainMenuTimer.ReadSec());
    // X
    if (time >= scoreboardComponent.m_mainMenuTimeout) {
        ChangeToMainMenu();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RestartStateClient::RenderGui() const
{
    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    F32 time = static_cast<F32>(scoreboardComponent.m_guiTimer.ReadSec());
    if (time >= 3.0f) {
        scoreboardComponent.m_guiTimer.Start();
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
    // X
    if (ImGui::Button(cancel)) {
        ChangeToResults();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RestartStateClient::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
void RestartStateClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

        // V
    case EventType::REWELCOME_RECEIVED: {
        ChangeToGameplay();
        break;
    }

        // X
    case EventType::PLAYER_REMOVED: {
        ChangeToMainMenu();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void RestartStateClient::ChangeToGameplay() const
{
    g_gameClient->GetFSM().ChangeState(g_gameClient->GetConfig().m_gameplaySceneName.c_str());
}

//----------------------------------------------------------------------------------------------------
void RestartStateClient::ChangeToResults() const
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    gameplayComponent.m_fsm.ChangeState("Results");
}

//----------------------------------------------------------------------------------------------------
void RestartStateClient::ChangeToMainMenu() const
{
    g_gameClient->GetFSM().ChangeState(g_gameClient->GetConfig().m_mainMenuSceneName.c_str());
}
}
