#include "ConnectStateClient.h"

#include "ClientComponent.h"
#include "ConfigClient.h"
#include "FSM.h"
#include "GameClient.h"
#include "GuiComponent.h"
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

    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameClient->GetMainMenuComponent();
    mainMenuComponent.lock()->m_helloTimer.Start();
    mainMenuComponent.lock()->m_guiTimer.Start();

    Event newEvent;
    newEvent.eventType = EventType::CONNECT_SOCKETS;
    g_gameClient->GetFSM().NotifyEvent(newEvent);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ConnectStateClient::Update() const
{
    Event newEvent;
    newEvent.eventType = EventType::CHECK_CONNECT;
    g_gameClient->GetFSM().NotifyEvent(newEvent);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool ConnectStateClient::RenderGui() const
{
    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameClient->GetMainMenuComponent();
    std::weak_ptr<GuiComponent> guiComponent = g_gameClient->GetGuiComponent();
    F32 guiCurrentTime = static_cast<F32>(mainMenuComponent.lock()->m_guiTimer.ReadSec());
    if (guiCurrentTime >= guiComponent.lock()->m_secondsBetweenEllipsis) {
        mainMenuComponent.lock()->m_guiTimer.Start();
    }
    F32 fractionSecondsBetweenEllipsis = guiComponent.lock()->m_secondsBetweenEllipsis / 3.0f;
    if (guiCurrentTime >= fractionSecondsBetweenEllipsis * 2.0f) {
        ImGui::Text("Connecting...");
    } else if (guiCurrentTime >= fractionSecondsBetweenEllipsis) {
        ImGui::Text("Connecting..");
    } else if (guiCurrentTime >= 0.0f) {
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

    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameClient->GetMainMenuComponent();
    mainMenuComponent.lock()->m_log = std::pair<std::string, LogTypes>();

    return true;
}

//----------------------------------------------------------------------------------------------------
void ConnectStateClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::SOCKETS_CONNECTED: {
        std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameClient->GetMainMenuComponent();
        F32 helloCurrentTime = static_cast<F32>(mainMenuComponent.lock()->m_helloTimer.ReadSec());
        if (helloCurrentTime >= mainMenuComponent.lock()->m_secondsBetweenHello) {
            Event newEvent;
            newEvent.eventType = EventType::SEND_HELLO;
            g_gameClient->GetFSM().NotifyEvent(newEvent);

            mainMenuComponent.lock()->m_helloTimer.Start();
        }
        break;
    }

        // V
    case EventType::WELCOME_RECEIVED: {
        ChangeToGameplay();
        break;
    }

        // X
    case EventType::UNWELCOME_RECEIVED: {
        Event newEvent;
        newEvent.eventType = EventType::DISCONNECT_SOCKETS;
        g_gameClient->GetFSM().NotifyEvent(newEvent);

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
    g_gameClient->GetFSM().ChangeState("Gameplay");
}

//----------------------------------------------------------------------------------------------------
void ConnectStateClient::ChangeToSetup() const
{
    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameClient->GetMainMenuComponent();
    mainMenuComponent.lock()->m_fsm.ChangeState("Setup");
}
}
