#include "ResultsStateClient.h"

#include "ClientComponent.h"
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
    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameClient->GetScoreboardComponent();
    F32 mainMenuCurrentTime = static_cast<F32>(scoreboardComponent.lock()->m_mainMenuTimer.ReadSec());
    // X
    if (mainMenuCurrentTime >= scoreboardComponent.lock()->m_mainMenuTimeout) {
        Event newEvent;
        newEvent.eventType = EventType::SEND_BYE;
        g_gameClient->GetFSM().NotifyEvent(newEvent);

        ChangeToMainMenu();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ResultsStateClient::RenderGui() const
{
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImVec2 framePadding = ImGui::GetStyle().FramePadding;
    ImVec2 itemSpacing = ImGui::GetStyle().ItemSpacing;

    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameClient->GetScoreboardComponent();
    if (scoreboardComponent.lock()->m_winnerPlayerID == INVALID_PLAYER_ID) {
        std::string tie = "Tie!";
        ImVec2 tieTextSize = ImGui::CalcTextSize(tie.c_str());
        ImGui::SetCursorPosX(contentRegionMax.x / 2.0f - tieTextSize.x / 2.0f);
        ImGui::Text(tie.c_str());
    } else {
        std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
        const bool isLocalPlayer = clientComponent.lock()->IsLocalPlayer(scoreboardComponent.lock()->m_winnerPlayerID);
        if (isLocalPlayer) {
            std::string youWin = "You win :)";
            ImVec2 youWinTextSize = ImGui::CalcTextSize(youWin.c_str());
            ImGui::SetCursorPosX(contentRegionMax.x / 2.0f - youWinTextSize.x / 2.0f);
            ImGui::Text(youWin.c_str());
        } else {
            std::string youLose = "You lose :(";
            ImVec2 youLoseTextSize = ImGui::CalcTextSize(youLose.c_str());
            ImGui::SetCursorPosX(contentRegionMax.x / 2.0f - youLoseTextSize.x / 2.0f);
            ImGui::Text(youLose.c_str());
        }
    }

    ImGui::Spacing();

    // TODO

    F32 mainMenuCurrentTime = static_cast<F32>(scoreboardComponent.lock()->m_mainMenuTimer.ReadSec());
    F32 mainMenuTimeLeft = scoreboardComponent.lock()->m_mainMenuTimeout - mainMenuCurrentTime;
    std::string mainMenuTimeLeftString = std::to_string(static_cast<U32>(std::ceil(mainMenuTimeLeft)));
    ImVec2 mainMenuTimeLeftTextSize = ImGui::CalcTextSize(mainMenuTimeLeftString.c_str());
    ImGui::SetCursorPosY(contentRegionMax.y - mainMenuTimeLeftTextSize.y);
    ImGui::Text(mainMenuTimeLeftString.c_str());

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
    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameClient->GetScoreboardComponent();
    scoreboardComponent.lock()->m_fsm.ChangeState("Restart");
}

//----------------------------------------------------------------------------------------------------
void ResultsStateClient::ChangeToMainMenu() const
{
    g_gameClient->GetFSM().ChangeState("Main Menu");
}
}
