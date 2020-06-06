#include "SetupStateServer.h"

#include "FSM.h"
#include "FileSystem.h"
#include "GameServer.h"
#include "MainMenuComponent.h"
#include "MapImporter.h"
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

    std::weak_ptr<WindowComponent> windowComponent = g_gameServer->GetWindowComponent();
    ImVec2 position = ImVec2(static_cast<F32>(windowComponent.lock()->m_currentResolution.x) / 2.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.y) / 2.0f);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImVec2 size = ImVec2(static_cast<F32>(windowComponent.lock()->m_currentResolution.y) / 2.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.x) / 2.0f);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    if (ImGui::Begin("Main Menu", nullptr, windowFlags)) {
        ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
        ImVec2 framePadding = ImGui::GetStyle().FramePadding;

        std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
        ImGui::InputText("Port", &serverComponent.lock()->m_port);

        ImGui::Spacing();

        if (ImGui::BeginCombo("Map", serverComponent.lock()->m_map.c_str())) {
            std::vector<std::string> entries = g_gameServer->GetFileSystem().GetFilesFromDirectory(MAPS_DIR, MAPS_EXTENSION);
            if (!entries.empty()) {
                for (const auto& entry : entries) {
                    std::string name = g_gameServer->GetFileSystem().GetName(entry);
                    if (ImGui::Selectable(name.c_str())) {
                        serverComponent.lock()->m_map = name;
                    }
                }

                if (ImGui::Selectable("Random")) {
                    serverComponent.lock()->m_map = "Random";
                }
            }

            ImGui::EndCombo();
        }

        ImGui::InputInt("Duration", &serverComponent.lock()->m_duration);

        std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameServer->GetMainMenuComponent();
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

        std::string startText = "Start";
        ImVec2 startTextSize = ImGui::CalcTextSize(startText.c_str());
        ImVec2 startButtonSize = ImVec2(startTextSize.x + framePadding.x * 2.0f, startTextSize.y + framePadding.y * 2.0f);
        ImGui::SetCursorPosX(contentRegionMax.x - startButtonSize.x);
        ImGui::SetCursorPosY(contentRegionMax.y - startButtonSize.y);
        // V
        if (ImGui::Button(startText.c_str())) {
            Event newEvent;
            newEvent.eventType = EventType::TRY_CONNECT;
            g_gameServer->GetFSM().NotifyEvent(newEvent);
        }

        ImGui::End();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool SetupStateServer::Exit() const
{
    ILOG("Exiting %s...", GetName().c_str());

    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameServer->GetMainMenuComponent();
    mainMenuComponent.lock()->m_log = std::pair<std::string, LogTypes>();

    return true;
}

//----------------------------------------------------------------------------------------------------
void SetupStateServer::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::TRY_CONNECT: {
        ImportMap();
        break;
    }

    case EventType::CONNECT_SUCCESSFUL: {
        ChangeToConnect();
        break;
    }

    case EventType::CONNECT_FAILED: {
        std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameServer->GetMainMenuComponent();
        mainMenuComponent.lock()->m_log = std::make_pair("Map failed", LogTypes::ELOG);
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void SetupStateServer::ChangeToConnect() const
{
    std::weak_ptr<MainMenuComponent> mainMenuComponent = g_gameServer->GetMainMenuComponent();
    mainMenuComponent.lock()->m_fsm.ChangeState("Connect");
}

//----------------------------------------------------------------------------------------------------
void SetupStateServer::ImportMap() const
{
    std::weak_ptr<ServerComponent> serverComponent = g_gameServer->GetServerComponent();
    if (serverComponent.lock()->m_map == "Random") {
        std::vector<std::string> entries = g_gameServer->GetFileSystem().GetFilesFromDirectory(MAPS_DIR, MAPS_EXTENSION);
        I32 min = 0;
        I32 max = static_cast<I32>(entries.size()) - 1;
        I32 random = RandomInt(min, max);
        std::string name = g_gameServer->GetFileSystem().GetName(entries.at(random));
        serverComponent.lock()->m_map = name;
    }

    std::string path = MAPS_DIR;
    path.append(serverComponent.lock()->m_map);
    path.append(MAPS_EXTENSION);
    MapImporter::Tilemap tilemap;
    const bool result = g_gameServer->GetMapImporter().Load(path, tilemap);

    Event newEvent;
    if (result) {
        newEvent.eventType = EventType::CONNECT_SUCCESSFUL;
    } else {
        newEvent.eventType = EventType::CONNECT_FAILED;
    }
    g_gameServer->GetFSM().NotifyEvent(newEvent);
}
}
