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
    // X
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
        std::weak_ptr<GameplayComponent> gameplayComponent = g_gameClient->GetGameplayComponent();
        F32 mainMenuCurrentTime = static_cast<F32>(gameplayComponent.lock()->m_mainMenuTimer.ReadSec());
        F32 mainMenuTimeLeft = gameplayComponent.lock()->m_mainMenuTimeout - mainMenuCurrentTime;
        ImGui::Text("%.0f", mainMenuTimeLeft);

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

        // V
    case EventType::PLAY_RECEIVED: {
        ChangeToPlay();
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
