#include "MainMenuStateClient.h"

#include "ComponentManager.h"
#include "Config.h"
#include "EntityManager.h"
#include "FSM.h"
#include "GameClient.h"
#include "GameplayComponent.h"
#include "MainMenuComponent.h"
#include "ResourceManager.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "TransformComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
const char* MainMenuStateClient::GetName() const
{
    return "MainMenu";
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateClient::Enter() const
{
    ILOG("Entering MainMenuStateClient...");

    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    mainMenuComponent.m_phase = MainMenuComponent::MainMenuPhase::SETUP;

    Entity background = g_gameClient->GetEntityManager().AddEntity();

    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
    std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().AddComponent<TransformComponent>(background);
    transformComponent.lock()->m_position = { static_cast<F32>(windowComponent.m_resolution.x) / 2.0f, static_cast<F32>(windowComponent.m_resolution.y) / 2.0f, 0.0f };

    std::weak_ptr<SpriteResource> spriteResource = g_gameClient->GetResourceManager().AddResource<SpriteResource>("mainMenuBackground.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().AddComponent<SpriteComponent>(background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

    return true;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateClient::Update() const
{
    GameplayComponent& gameplayComponent = g_gameClient->GetGameplayComponent();
    if (gameplayComponent.m_phase != GameplayComponent::GameplayPhase::NONE) {
        g_gameClient->GetFSM().ChangeState(g_gameClient->GetConfig().m_onlineSceneName.c_str());
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateClient::RenderGui() const
{
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoSavedSettings;

    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
    ImVec2 position = ImVec2(static_cast<F32>(windowComponent.m_resolution.x) / 2.0f, static_cast<F32>(windowComponent.m_resolution.y) / 2.0f);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImVec2 size = ImVec2(static_cast<F32>(windowComponent.m_resolution.y) / 2.0f, static_cast<F32>(windowComponent.m_resolution.x) / 2.0f);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    if (ImGui::Begin("Client", nullptr, windowFlags)) {
        MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
        switch (mainMenuComponent.m_phase) {
        case MainMenuComponent::MainMenuPhase::SETUP: {
            RenderStartGui(mainMenuComponent);
            break;
        }

        case MainMenuComponent::MainMenuPhase::CONNECT: {
            RenderConnectGui(mainMenuComponent);
            break;
        }

        default: {
            break;
        }
        }

        ImGui::End();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateClient::Exit() const
{
    ILOG("Exiting MainMenuStateClient...");

    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    mainMenuComponent.m_phase = MainMenuComponent::MainMenuPhase::NONE;

    g_gameClient->GetEntityManager().ClearEntities();

    return true;
}

//----------------------------------------------------------------------------------------------------
void MainMenuStateClient::RenderStartGui(MainMenuComponent& mainMenuComponent) const
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    ImGui::InputText("IP", &clientComponent.m_ip[0], DEFAULT_INPUT_SIZE);
    ImGui::InputText("Port", &clientComponent.m_port[0], DEFAULT_INPUT_SIZE);

    ImGui::InputText("Name", &clientComponent.m_name[0], DEFAULT_INPUT_SIZE);

    ImVec2 textSize = ImGui::CalcTextSize("Start");
    ImVec2 framePadding = ImGui::GetStyle().FramePadding;
    ImVec2 buttonSize = ImVec2(textSize.x + framePadding.x * 2.0f, textSize.y + framePadding.y * 2.0f);
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImGui::SetCursorPosX(contentRegionMax.x - buttonSize.x);
    ImGui::SetCursorPosY(contentRegionMax.y - buttonSize.y);
    if (ImGui::Button("Start")) {
        mainMenuComponent.m_phase = MainMenuComponent::MainMenuPhase::CONNECT;
        mainMenuComponent.m_timer.Start();
    }
}

//----------------------------------------------------------------------------------------------------
void MainMenuStateClient::RenderConnectGui(MainMenuComponent& mainMenuComponent) const
{
    F32 time = static_cast<F32>(mainMenuComponent.m_timer.ReadSec());
    if (time >= 3.0f) {
        mainMenuComponent.m_timer.Start();
    }

    if (time >= 2.0f) {
        ImGui::Text("Connecting...");
    } else if (time >= 1.0f) {
        ImGui::Text("Connecting..");
    } else if (time >= 0.0f) {
        ImGui::Text("Connecting.");
    }

    ImVec2 textSize = ImGui::CalcTextSize("Cancel");
    ImVec2 framePadding = ImGui::GetStyle().FramePadding;
    ImVec2 buttonSize = ImVec2(textSize.x + framePadding.x * 2.0f, textSize.y + framePadding.y * 2.0f);
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImGui::SetCursorPosX(contentRegionMax.x - buttonSize.x);
    ImGui::SetCursorPosY(contentRegionMax.y - buttonSize.y);
    if (ImGui::Button("Cancel")) {
        mainMenuComponent.m_phase = MainMenuComponent::MainMenuPhase::SETUP;
    }
}
}
