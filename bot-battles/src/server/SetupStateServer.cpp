#include "SetupStateServer.h"

#include "GameServer.h"
#include "MainMenuComponent.h"
#include "ServerComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string SetupStateServer::GetName() const
{
    return "Setup";
}

//----------------------------------------------------------------------------------------------------
bool SetupStateServer::Enter() const
{
    ILOG("Entering %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool SetupStateServer::RenderGui() const
{
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoSavedSettings;

    WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
    glm::uvec2 resolution = windowComponent.GetResolution();
    ImVec2 position = ImVec2(static_cast<F32>(resolution.x) / 2.0f, static_cast<F32>(resolution.y) / 2.0f);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImVec2 size = ImVec2(static_cast<F32>(resolution.y) / 2.0f, static_cast<F32>(resolution.x) / 2.0f);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    ImGui::SetNextWindowFocus();

    if (ImGui::Begin("Main Menu", nullptr, windowFlags)) {
        ServerComponent& serverComponent = g_gameServer->GetServerComponent();
        ImGui::InputText("Port", &serverComponent.m_port);

        if (ImGui::BeginCombo("Map", serverComponent.m_map.c_str())) {
            std::vector<std::string> entries = g_gameServer->GetFileSystem().GetFilesFromDirectory(MAPS_DIR);
            for (const auto& entry : entries) {
                if (ImGui::Selectable(entry.c_str())) {
                    serverComponent.m_map = entry;
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
            ChangeToConnect();
        }

        ImGui::End();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool SetupStateServer::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    return true;
}

//----------------------------------------------------------------------------------------------------
void SetupStateServer::ChangeToConnect() const
{
    MainMenuComponent& mainMenuComponent = g_gameServer->GetMainMenuComponent();
    mainMenuComponent.m_fsm.ChangeState("Connect");
}
}
