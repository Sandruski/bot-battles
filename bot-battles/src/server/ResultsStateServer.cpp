#include "ResultsStateServer.h"

#include "ConfigServer.h"
#include "FSM.h"
#include "GameServer.h"
#include "ScoreboardComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string ResultsStateServer::GetName() const
{
    return "Results";
}

//----------------------------------------------------------------------------------------------------
bool ResultsStateServer::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    Event newEvent;
    newEvent.eventType = EventType::SEND_RESULTS;
    g_gameServer->GetFSM().NotifyEvent(newEvent);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ResultsStateServer::RenderGui() const
{
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoSavedSettings;

    WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
    ImVec2 position = ImVec2(static_cast<F32>(windowComponent.m_resolution.x) / 2.0f, static_cast<F32>(windowComponent.m_resolution.y) / 2.0f);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImVec2 size = ImVec2(static_cast<F32>(windowComponent.m_resolution.y) / 2.0f, static_cast<F32>(windowComponent.m_resolution.x) / 2.0f);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    if (ImGui::Begin("Server", nullptr, windowFlags)) {
        ScoreboardComponent& scoreboardComponent = g_gameServer->GetScoreboardComponent();
        if (scoreboardComponent.m_winnerPlayerID == INVALID_PLAYER_ID) {
            ImGui::Text("Match ended because a player left...");
        } else {
            U32 playerNumber = scoreboardComponent.m_winnerPlayerID + 1;
            ImGui::Text("Player %u wins :)", playerNumber);
        }

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
            g_gameServer->GetFSM().NotifyEvent(newEvent);

            ChangeToMainMenu();
        }

        ImGui::End();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ResultsStateServer::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
void ResultsStateServer::ChangeToRestart() const
{
    ScoreboardComponent& scoreboardComponent = g_gameServer->GetScoreboardComponent();
    scoreboardComponent.m_fsm.ChangeState("Restart");
}

//----------------------------------------------------------------------------------------------------
void ResultsStateServer::ChangeToMainMenu() const
{
    g_gameServer->GetFSM().ChangeState(g_gameServer->GetConfig().m_mainMenuSceneName.c_str());
}
}
