#include "ResultsStateServer.h"

#include "ConfigServer.h"
#include "FSM.h"
#include "GameServer.h"
#include "ScoreboardComponent.h"

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
