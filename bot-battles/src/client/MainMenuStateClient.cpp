#include "MainMenuStateClient.h"

#include "ClientComponent.h"
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
    ILOG("Entering %s...", GetName());

    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    mainMenuComponent.m_phase = MainMenuComponent::MainMenuPhase::SETUP;

    Entity background = g_gameClient->GetEntityManager().AddEntity();
    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
    std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().AddComponent<TransformComponent>(background);
    transformComponent.lock()->m_position = { static_cast<F32>(windowComponent.m_resolution.x) / 2.0f, static_cast<F32>(windowComponent.m_resolution.y) / 2.0f, static_cast<F32>(LayerType::BACKGROUND) };
    std::weak_ptr<SpriteResource> spriteResource = g_gameClient->GetResourceManager().AddResource<SpriteResource>("mainMenuBackground.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().AddComponent<SpriteComponent>(background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

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

    if (ImGui::Begin(GetName(), nullptr, windowFlags)) {
        MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
        switch (mainMenuComponent.m_phase) {
        case MainMenuComponent::MainMenuPhase::SETUP: {
            RenderSetupGui(mainMenuComponent);
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
    ILOG("Exiting %s...", GetName());

    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    mainMenuComponent.m_phase = MainMenuComponent::MainMenuPhase::NONE;

    g_gameClient->GetEntityManager().ClearEntities();

    return true;
}

//----------------------------------------------------------------------------------------------------
void MainMenuStateClient::OnNotify(const Event& event)
{
    switch (event.eventType) {

    case EventType::WELCOME_RECEIVED: {
        OnWelcomeReceived();
        break;
    }

    default: {
        break;
    }
    }
}

//----------------------------------------------------------------------------------------------------
void MainMenuStateClient::RenderSetupGui(MainMenuComponent& mainMenuComponent) const
{
    ClientComponent& clientComponent = g_gameClient->GetClientComponent();
    ImGui::InputText("IP", &clientComponent.m_ip[0], DEFAULT_INPUT_SIZE);
    ImGui::InputText("Port", &clientComponent.m_port[0], DEFAULT_INPUT_SIZE);

    ImGui::InputText("Name", &clientComponent.m_name[0], DEFAULT_INPUT_SIZE);

    const char* start = "Start";
    ImVec2 textSize = ImGui::CalcTextSize(start);
    ImVec2 framePadding = ImGui::GetStyle().FramePadding;
    ImVec2 buttonSize = ImVec2(textSize.x + framePadding.x * 2.0f, textSize.y + framePadding.y * 2.0f);
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImGui::SetCursorPosX(contentRegionMax.x - buttonSize.x);
    ImGui::SetCursorPosY(contentRegionMax.y - buttonSize.y);
    if (ImGui::Button(start)) {
        mainMenuComponent.m_phase = MainMenuComponent::MainMenuPhase::CONNECT;
        mainMenuComponent.m_timer.Start();

        clientComponent.m_connect = true;
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

    const char* cancel = "Cancel";
    ImVec2 textSize = ImGui::CalcTextSize(cancel);
    ImVec2 framePadding = ImGui::GetStyle().FramePadding;
    ImVec2 buttonSize = ImVec2(textSize.x + framePadding.x * 2.0f, textSize.y + framePadding.y * 2.0f);
    ImVec2 contentRegionMax = ImGui::GetWindowContentRegionMax();
    ImGui::SetCursorPosX(contentRegionMax.x - buttonSize.x);
    ImGui::SetCursorPosY(contentRegionMax.y - buttonSize.y);
    if (ImGui::Button(cancel)) {
        mainMenuComponent.m_phase = MainMenuComponent::MainMenuPhase::SETUP;

        ClientComponent& clientComponent = g_gameClient->GetClientComponent();
        clientComponent.m_disconnect = true;
    }
}

//----------------------------------------------------------------------------------------------------
void MainMenuStateClient::OnWelcomeReceived() const
{
    g_gameClient->GetFSM().ChangeState(g_gameClient->GetConfig().m_onlineSceneName.c_str());
}
}
