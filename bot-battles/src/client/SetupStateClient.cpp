#include "SetupStateClient.h"

#include "ClientComponent.h"
#include "FileSystem.h"
#include "GameClient.h"
#include "MainMenuComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string SetupStateClient::GetName() const
{
    return "Setup";
}

//----------------------------------------------------------------------------------------------------
bool SetupStateClient::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool SetupStateClient::RenderGui() const
{
    std::weak_ptr<ClientComponent> clientComponent = g_gameClient->GetClientComponent();
    ImGui::InputText("IP", &clientComponent.lock()->m_ip);
    ImGui::InputText("Port", &clientComponent.lock()->m_port);

    ImGui::InputText("Name", &clientComponent.lock()->m_name);

    if (ImGui::BeginCombo("Script", clientComponent.lock()->m_script.c_str())) {
        std::vector<std::string> entries = g_gameClient->GetFileSystem().GetFilesFromDirectory(BOTS_SCRIPTS_DIR, SCRIPTS_EXTENSION);
        for (const auto& entry : entries) {
            std::string name = g_gameClient->GetFileSystem().GetName(entry);
            if (ImGui::Selectable(name.c_str())) {
                clientComponent.lock()->m_script = name;
            }
        }
        ImGui::EndCombo();
    }

    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameClient->GetMainMenuComponent();
    LogTypes logType = mainMenuComponent.lock()->m_log.second;
    ImVec4 color;
    switch (logType) {
    case LogTypes::ILOG: {
        color = ImVec4(Green.r, Green.g, Green.b, Green.a);
        break;
    }
    case LogTypes::WLOG: {
        color = ImVec4(Yellow.r, Yellow.g, Yellow.b, Yellow.a);
        break;
    }
    case LogTypes::ELOG: {
        color = ImVec4(Red.r, Red.g, Red.b, Red.a);
        break;
    }
    default: {
        break;
    }
    }
    ImGui::TextColored(color, mainMenuComponent.lock()->m_log.first.c_str());

    const char* start = "Start";
    ImVec2 textSize = ImGui::CalcTextSize(start);
    ImVec2 framePadding = ImGui::GetStyle().FramePadding;
    ImVec2 buttonSize = ImVec2(textSize.x + framePadding.x * 2.0f, textSize.y + framePadding.y * 2.0f);
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImGui::SetCursorPosX(contentRegionMax.x - buttonSize.x);
    ImGui::SetCursorPosY(contentRegionMax.y - buttonSize.y);
    // V
    if (ImGui::Button(start)) {
        Event newEvent;
        newEvent.eventType = EventType::TRY_CONNECT;
        g_gameClient->GetFSM().NotifyEvent(newEvent);
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool SetupStateClient::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameClient->GetMainMenuComponent();
    mainMenuComponent.lock()->m_log = std::pair<std::string, LogTypes>();

    return true;
}

//----------------------------------------------------------------------------------------------------
void SetupStateClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::CONNECT_SUCCESSFUL: {
        ChangeToConnect();
        break;
    }

    case EventType::CONNECT_FAILED: {
        std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameClient->GetMainMenuComponent();
        mainMenuComponent.lock()->m_log = std::make_pair("Script failed", LogTypes::ELOG);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void SetupStateClient::ChangeToConnect() const
{
    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameClient->GetMainMenuComponent();
    mainMenuComponent.lock()->m_fsm.ChangeState("Connect");
}
}
