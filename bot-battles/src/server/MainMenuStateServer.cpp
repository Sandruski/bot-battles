#include "MainMenuStateServer.h"

#include "ComponentManager.h"
#include "Config.h"
#include "EntityManager.h"
#include "FSM.h"
#include "FileSystem.h"
#include "GameServer.h"
#include "GameplayStateServer.h"
#include "MainMenuComponent.h"
#include "ResourceManager.h"
#include "ServerComponent.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
const char* MainMenuStateServer::GetName() const
{
    return "MainMenu";
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateServer::Enter() const
{
    ILOG("Entering MainMenuStateServer...");

    MainMenuComponent& mainMenuComponent = g_gameServer->GetMainMenuComponent();
    mainMenuComponent.m_phase = MainMenuComponent::MainMenuPhase::SETUP;

    Entity background = g_gameServer->GetEntityManager().AddEntity();

    WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
    std::weak_ptr<TransformComponent> transformComponent = g_gameServer->GetComponentManager().AddComponent<TransformComponent>(background);
    transformComponent.lock()->m_position = { static_cast<F32>(windowComponent.m_resolution.x) / 2.0f, static_cast<F32>(windowComponent.m_resolution.y) / 2.0f, static_cast<F32>(LayerType::BACKGROUND) };

    std::weak_ptr<SpriteResource> spriteResource = g_gameServer->GetResourceManager().AddResource<SpriteResource>("mainMenuBackground.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameServer->GetComponentManager().AddComponent<SpriteComponent>(background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

    return true;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateServer::RenderGui() const
{
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoSavedSettings;

    WindowComponent& windowComponent = g_gameServer->GetWindowComponent();
    ImVec2 position = ImVec2(static_cast<F32>(windowComponent.m_resolution.x) / 2.0f, static_cast<F32>(windowComponent.m_resolution.y) / 2.0f);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImVec2 size = ImVec2(static_cast<F32>(windowComponent.m_resolution.y) / 2.0f, static_cast<F32>(windowComponent.m_resolution.x) / 2.0f);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    if (ImGui::Begin("Server", nullptr, windowFlags)) {

        RenderStartGui();

        ImGui::End();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateServer::Exit() const
{
    ILOG("Exiting MainMenuStateServer...");

    MainMenuComponent& mainMenuComponent = g_gameServer->GetMainMenuComponent();
    mainMenuComponent.m_phase = MainMenuComponent::MainMenuPhase::NONE;

    g_gameServer->GetEntityManager().ClearEntities();

    return true;
}

//----------------------------------------------------------------------------------------------------
void MainMenuStateServer::RenderStartGui() const
{
    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    ImGui::InputText("Port", &serverComponent.m_port[0], DEFAULT_INPUT_SIZE);

    if (ImGui::BeginCombo("Map", serverComponent.m_map.c_str())) {
        std::vector<std::string> entries = g_gameServer->GetFileSystem().GetFilesFromDirectory(MAPS_DIR);
        for (const auto& entry : entries) {
            if (ImGui::Selectable(entry.c_str())) {
                serverComponent.m_map = entry;
            }
        }
        ImGui::EndCombo();
    }

    ImVec2 textSize = ImGui::CalcTextSize("Start");
    ImVec2 framePadding = ImGui::GetStyle().FramePadding;
    ImVec2 buttonSize = ImVec2(textSize.x + framePadding.x * 2.0f, textSize.y + framePadding.y * 2.0f);
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImGui::SetCursorPosX(contentRegionMax.x - buttonSize.x);
    ImGui::SetCursorPosY(contentRegionMax.y - buttonSize.y);
    if (ImGui::Button("Start")) {
        g_gameServer->GetFSM().ChangeState(g_gameServer->GetConfig().m_onlineSceneName.c_str());
    }
}
}
