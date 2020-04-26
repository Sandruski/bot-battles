#include "SetupStateClient.h"

#include "ClientComponent.h"
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
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    ImGui::InputText("IP", &clientComponent.m_ip);
    ImGui::InputText("Port", &clientComponent.m_port);

    ImGui::InputText("Name", &clientComponent.m_name);

    if (ImGui::BeginCombo("Script", clientComponent.m_script.c_str())) {
        std::vector<std::string> entries = g_gameClient->GetFileSystem().GetFilesFromDirectory(SCRIPTS_DIR, SCRIPTS_EXTENSION);
        for (const auto& entry : entries) {
            std::string name = g_gameClient->GetFileSystem().GetName(entry);
            if (ImGui::Selectable(name.c_str())) {
                clientComponent.m_script = name;
            }
        }
        ImGui::EndCombo();
    }

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
        // TODO
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
    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    mainMenuComponent.m_fsm.ChangeState("Connect");
}
}
