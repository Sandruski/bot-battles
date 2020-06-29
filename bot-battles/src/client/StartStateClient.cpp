#include "StartStateClient.h"

#include "ConfigClient.h"
#include "FSM.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string StartStateClient::GetName() const
{
    return "Start";
}

//----------------------------------------------------------------------------------------------------
bool StartStateClient::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool StartStateClient::Update() const
{
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    F32 mainMenuCurrentTime = static_cast<F32>(gameplayComponent.lock()->m_mainMenuTimer.ReadSec());
    if (mainMenuCurrentTime >= gameplayComponent.lock()->m_mainMenuTimeout) {
        Event newEvent;
        newEvent.eventType = EventType::SEND_BYE;
        g_gameClient->GetFSM().NotifyEvent(newEvent);

        ChangeToMainMenu();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool StartStateClient::RenderGui() const
{
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoSavedSettings;

    std::weak_ptr<WindowComponent> windowComponent = g_gameClient->GetWindowComponent();
    ImVec2 position = ImVec2(static_cast<F32>(windowComponent.lock()->m_currentResolution.x) / 2.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.y) / 2.0f);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImVec2 size = ImVec2(static_cast<F32>(windowComponent.lock()->m_currentResolution.y) / 2.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.x) / 2.0f);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    if (ImGui::Begin(GetName().c_str(), nullptr, windowFlags)) {
        ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
        ImVec2 framePadding = ImGui::GetStyle().FramePadding;

        std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
        F32 mainMenuCurrentTime = static_cast<F32>(gameplayComponent.lock()->m_mainMenuTimer.ReadSec());
        F32 mainMenuTimeLeft = gameplayComponent.lock()->m_mainMenuTimeout - mainMenuCurrentTime;
        std::string mainMenuTimeLeftString = std::to_string(static_cast<U32>(std::ceil(mainMenuTimeLeft)));
        ImVec2 mainMenuTimeLeftTextSize = ImGui::CalcTextSize(mainMenuTimeLeftString.c_str());
        ImGui::SetCursorPosY(contentRegionMax.y - mainMenuTimeLeftTextSize.y);
        ImGui::Text(mainMenuTimeLeftString.c_str());

        std::string mainMenuString = "Main menu";
        ImVec2 mainMenuTextSize = ImGui::CalcTextSize(mainMenuString.c_str());
        ImVec2 mainMenuButtonSize = ImVec2(mainMenuTextSize.x + framePadding.x * 2.0f, mainMenuTextSize.y + framePadding.y * 2.0f);
        ImGui::SetCursorPosX(contentRegionMax.x - mainMenuButtonSize.x);
        ImGui::SetCursorPosY(contentRegionMax.y - mainMenuButtonSize.y);
        if (ImGui::Button(mainMenuString.c_str())) {
            Event newEvent;
            newEvent.eventType = EventType::SEND_BYE;
            g_gameClient->GetFSM().NotifyEvent(newEvent);

            ChangeToMainMenu();
        }

        ImGui::End();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool StartStateClient::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
void StartStateClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::PLAY_RECEIVED: {
        ChangeToPlay();
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
void StartStateClient::ChangeToPlay() const
{
    std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
    gameplayComponent.lock()->m_fsm.ChangeState("Play");
}

//----------------------------------------------------------------------------------------------------
void StartStateClient::ChangeToMainMenu() const
{
    g_gameClient->GetFSM().ChangeState("Main Menu");
}
}
