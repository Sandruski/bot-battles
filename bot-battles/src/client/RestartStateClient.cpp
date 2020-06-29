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
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImVec2 framePadding = ImGui::GetStyle().FramePadding;

    std::weak_ptr<ScoreboardComponent> scoreboardComponent = g_gameClient->GetScoreboardComponent();
    F32 guiCurrentTime = static_cast<F32>(scoreboardComponent.lock()->m_guiTimer.ReadSec());
    std::weak_ptr<GuiComponent> guiComponent = g_gameClient->GetGuiComponent();
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

    F32 mainMenuCurrentTime = static_cast<F32>(scoreboardComponent.lock()->m_mainMenuTimer.ReadSec());
    F32 mainMenuTimeLeft = scoreboardComponent.lock()->m_mainMenuTimeout - mainMenuCurrentTime;
    std::string mainMenuTimeLeftString = std::to_string(static_cast<U32>(std::ceil(mainMenuTimeLeft)));
    ImVec2 mainMenuTimeLeftTextSize = ImGui::CalcTextSize(mainMenuTimeLeftString.c_str());
    ImGui::SetCursorPosY(contentRegionMax.y - mainMenuTimeLeftTextSize.y);
    ImGui::Text(mainMenuTimeLeftString.c_str());

    std::string cancelString = "Cancel";
    ImVec2 cancelTextSize = ImGui::CalcTextSize(cancelString.c_str());
    ImVec2 cancelButtonSize = ImVec2(cancelTextSize.x + framePadding.x * 2.0f, cancelTextSize.y + framePadding.y * 2.0f);
    ImGui::SetCursorPosX(contentRegionMax.x - cancelButtonSize.x);
    ImGui::SetCursorPosY(contentRegionMax.y - cancelButtonSize.y);
    if (ImGui::Button(cancelString.c_str())) {
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

    case EventType::REWELCOME_RECEIVED: {
        ChangeToGameplay();
        break;
    }

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
