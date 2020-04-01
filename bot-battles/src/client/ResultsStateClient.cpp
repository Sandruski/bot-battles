#include "ResultsStateClient.h"

#include "ConfigClient.h"
#include "GameClient.h"
#include "ScoreboardComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string ResultsStateClient::GetName() const
{
    return "Results";
}

//----------------------------------------------------------------------------------------------------
bool ResultsStateClient::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ResultsStateClient::Update() const
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
bool ResultsStateClient::RenderGui() const
{
    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    F32 time = static_cast<F32>(scoreboardComponent.m_mainMenuTimer.ReadSec());
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
    // V
    if (ImGui::Button(playAgain)) {
        ChangeToRestart();
    }
    ImGui::SetCursorPosX(contentRegionMax.x - mainMenuButtonSize.x);
    ImGui::SetCursorPosY(contentRegionMax.y - mainMenuButtonSize.y);
    // X
    if (ImGui::Button(mainMenu)) {
        Event newEvent;
        newEvent.eventType = EventType::SEND_BYE;
        g_gameClient->GetFSM().NotifyEvent(newEvent);

        ChangeToMainMenu();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ResultsStateClient::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
void ResultsStateClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

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
void ResultsStateClient::ChangeToRestart() const
{
    ScoreboardComponent& scoreboardComponent = g_gameClient->GetScoreboardComponent();
    scoreboardComponent.m_fsm.ChangeState("Restart");
}

//----------------------------------------------------------------------------------------------------
void ResultsStateClient::ChangeToMainMenu() const
{
    g_gameClient->GetFSM().ChangeState(g_gameClient->GetConfig().m_mainMenuSceneName.c_str());
}
}