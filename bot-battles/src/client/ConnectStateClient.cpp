#include "ConnectStateClient.h"

#include "ClientComponent.h"
#include "ConfigClient.h"
#include "FSM.h"
#include "GameClient.h"
#include "MainMenuComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string ConnectStateClient::GetName() const
{
    return "Connect";
}

//----------------------------------------------------------------------------------------------------
bool ConnectStateClient::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    mainMenuComponent.m_guiTimer.Start();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ConnectStateClient::Update() const
{
    Event newEvent;
    newEvent.eventType = EventType::CONNECT_SOCKETS;
    g_gameClient->GetFSM().NotifyEvent(newEvent);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ConnectStateClient::RenderGui() const
{
    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    F32 time = static_cast<F32>(mainMenuComponent.m_guiTimer.ReadSec());
    if (time >= 3.0f) {
        mainMenuComponent.m_guiTimer.Start();
    }

    if (time >= 2.0f) {
        ImGui::Text("Connecting...");
    } else if (time >= 1.0f) {
        ImGui::Text("Connecting..");
    } else if (time >= 0.0f) {
        ImGui::Text("Connecting.");
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
        Event newEvent;
        newEvent.eventType = EventType::DISCONNECT_SOCKETS;
        g_gameClient->GetFSM().NotifyEvent(newEvent);

        ChangeToSetup();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ConnectStateClient::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
void ConnectStateClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::SOCKETS_CONNECTED: {
        Event newEvent;
        newEvent.eventType = EventType::SEND_HELLO;
        g_gameClient->GetFSM().NotifyEvent(newEvent);
        break;
    }

        // V
    case EventType::WELCOME_RECEIVED: {
        ChangeToGameplay();
        break;
    }

        // X
    case EventType::PLAYER_REMOVED: {
        ChangeToSetup();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void ConnectStateClient::ChangeToGameplay() const
{
    g_gameClient->GetFSM().ChangeState(g_gameClient->GetConfig().m_gameplaySceneName.c_str());
}

//----------------------------------------------------------------------------------------------------
void ConnectStateClient::ChangeToSetup() const
{
    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    mainMenuComponent.m_fsm.ChangeState("Setup");
}
}
