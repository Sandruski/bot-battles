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

    std::weak_ptr<WindowComponent> windowComponent = g_gameServer->GetWindowComponent();
    ImVec2 position = ImVec2(static_cast<F32>(windowComponent.lock()->m_currentResolution.x) / 2.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.y) / 2.0f);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImVec2 size = ImVec2(static_cast<F32>(windowComponent.lock()->m_currentResolution.y) / 2.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.x) / 2.0f);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    if (ImGui::Begin("Scoreboard", nullptr, windowFlags)) {
        ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
        ImVec2 framePadding = ImGui::GetStyle().FramePadding;
        ImVec2 itemSpacing = ImGui::GetStyle().ItemSpacing;

        std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameServer->GetScoreboardComponent();
        if (scoreboardComponent.lock()->m_winnerPlayerID == INVALID_PLAYER_ID) {
            std::string tieString = "Tie!";
            ImVec2 tieStringTextSize = ImGui::CalcTextSize(tieString.c_str());
            ImGui::SetCursorPosX(contentRegionMax.x / 2.0f - tieStringTextSize.x / 2.0f);
            ImGui::Text(tieString.c_str());
        } else {
            U32 playerNumber = scoreboardComponent.lock()->m_winnerPlayerID + 1;
            std::string winnerString = "Player ";
            winnerString.append(std::to_string(playerNumber));
            winnerString.append(" wins :)");
            ImVec2 winnerStringTextSize = ImGui::CalcTextSize(winnerString.c_str());
            ImGui::SetCursorPosX(contentRegionMax.x / 2.0f - winnerStringTextSize.x / 2.0f);
            ImGui::Text(winnerString.c_str());
        }

        std::string playAgainString = "Play again";
        std::string mainMenuString = "Main menu";
        ImVec2 playAgainTextSize = ImGui::CalcTextSize(playAgainString.c_str());
        ImVec2 mainMenuTextSize = ImGui::CalcTextSize(mainMenuString.c_str());
        ImVec2 playAgainButtonSize = ImVec2(playAgainTextSize.x + framePadding.x * 2.0f, playAgainTextSize.y + framePadding.y * 2.0f);
        ImVec2 mainMenuButtonSize = ImVec2(mainMenuTextSize.x + framePadding.x * 2.0f, mainMenuTextSize.y + framePadding.y * 2.0f);
        ImGui::SetCursorPosX(contentRegionMax.x - playAgainButtonSize.x - mainMenuButtonSize.x - itemSpacing.x);
        ImGui::SetCursorPosY(contentRegionMax.y - playAgainButtonSize.y);
        // V
        if (ImGui::Button(playAgainString.c_str())) {
            ChangeToRestart();
        }
        ImGui::SetCursorPosX(contentRegionMax.x - mainMenuButtonSize.x);
        ImGui::SetCursorPosY(contentRegionMax.y - mainMenuButtonSize.y);
        // X
        if (ImGui::Button(mainMenuString.c_str())) {
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
    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameServer->GetScoreboardComponent();
    scoreboardComponent.lock()->m_fsm.ChangeState("Restart");
}

//----------------------------------------------------------------------------------------------------
void ResultsStateServer::ChangeToMainMenu() const
{
    g_gameServer->GetFSM().ChangeState("Main Menu");
}
}
