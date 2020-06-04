#include "RestartStateClient.h"

#include "ConfigClient.h"
#include "GameClient.h"
#include "GuiComponent.h"
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

    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameClient->GetScoreboardComponent();
    scoreboardComponent.lock()->m_reHelloTimer.Start();
    scoreboardComponent.lock()->m_guiTimer.Start();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool RestartStateClient::Update() const
{
    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameClient->GetScoreboardComponent();
    F32 reHelloCurrentTime = static_cast<F32>(scoreboardComponent.lock()->m_reHelloTimer.ReadSec());
    if (reHelloCurrentTime >= scoreboardComponent.lock()->m_secondsBetweenReHello) {
        Event newEvent;
        newEvent.eventType = EventType::SEND_REHELLO;
        g_gameClient->GetFSM().NotifyEvent(newEvent);

        scoreboardComponent.lock()->m_reHelloTimer.Start();
    }

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
bool RestartStateClient::RenderGui() const
{
    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameClient->GetScoreboardComponent();
    F32 mainMenuCurrentTime = static_cast<F32>(scoreboardComponent.lock()->m_mainMenuTimer.ReadSec());
    F32 mainMenuTimeLeft = scoreboardComponent.lock()->m_mainMenuTimeout - mainMenuCurrentTime;
    ImGui::Text("%.0f", mainMenuTimeLeft);

    std::weak_ptr<GuiComponent> guiComponent = g_gameClient->GetGuiComponent();
    F32 guiCurrentTime = static_cast<F32>(scoreboardComponent.lock()->m_guiTimer.ReadSec());
    if (guiCurrentTime >= guiComponent.lock()->m_secondsBetweenEllipsis) {
        scoreboardComponent.lock()->m_guiTimer.Start();
    }
    F32 fractionSecondsBetweenEllipsis = guiComponent.lock()->m_secondsBetweenEllipsis / 3.0f;
    if (guiCurrentTime >= fractionSecondsBetweenEllipsis * 2.0f) {
        ImGui::Text("Waiting...");
    } else if (guiCurrentTime >= fractionSecondsBetweenEllipsis) {
        ImGui::Text("Waiting..");
    } else if (guiCurrentTime >= 0.0f) {
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
    g_gameClient->GetFSM().ChangeState("Gameplay");
}

//----------------------------------------------------------------------------------------------------
void RestartStateClient::ChangeToResults() const
{
    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameClient->GetScoreboardComponent();
    scoreboardComponent.lock()->m_fsm.ChangeState("Results");
}

//----------------------------------------------------------------------------------------------------
void RestartStateClient::ChangeToMainMenu() const
{
    g_gameClient->GetFSM().ChangeState("Main Menu");
}
}
