#include "MainMenuStateClient.h"

#include "ComponentManager.h"
#include "ConnectStateClient.h"
#include "EntityManager.h"
#include "GameClient.h"
#include "MainMenuComponent.h"
#include "SetupStateClient.h"
#include "SpriteComponent.h"
#include "SpriteResource.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string MainMenuStateClient::GetName() const
{
    return "Main Menu";
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateClient::Create() const
{
    bool ret = false;

    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    ret = mainMenuComponent.m_fsm.RegisterState<SetupStateClient>();
    if (!ret) {
        return ret;
    }
    ret = mainMenuComponent.m_fsm.RegisterState<ConnectStateClient>();
    if (!ret) {
        return ret;
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateClient::Enter() const
{
    // Scene
    Entity background = g_gameClient->GetEntityManager().AddEntity();
    std::weak_ptr<TransformComponent> transformComponent = g_gameClient->GetComponentManager().AddComponent<TransformComponent>(background);
    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
    transformComponent.lock()->m_position += static_cast<glm::vec2>(windowComponent.m_baseResolution) / 2.0f;
    transformComponent.lock()->m_layerType = LayerType::BACKGROUND;
    std::weak_ptr<SpriteResource> spriteResource = g_gameClient->GetResourceManager().AddResource<SpriteResource>("mainMenuBackground.png", TEXTURES_DIR, true);
    std::weak_ptr<SpriteComponent> spriteComponent = g_gameClient->GetComponentManager().AddComponent<SpriteComponent>(background);
    spriteComponent.lock()->m_spriteResource = spriteResource;

    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    return mainMenuComponent.m_fsm.ChangeState("Setup");
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateClient::Update() const
{
    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    return mainMenuComponent.m_fsm.Update();
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateClient::RenderGui() const
{
    bool ret = false;

    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoSavedSettings;

    WindowComponent& windowComponent = g_gameClient->GetWindowComponent();
    glm::uvec2 resolution = windowComponent.GetResolution();
    ImVec2 position = ImVec2(static_cast<F32>(resolution.x) / 2.0f, static_cast<F32>(resolution.y) / 2.0f);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImVec2 size = ImVec2(static_cast<F32>(resolution.y) / 2.0f, static_cast<F32>(resolution.x) / 2.0f);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    if (ImGui::Begin(GetName().c_str(), nullptr, windowFlags)) {
        MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
        ret = mainMenuComponent.m_fsm.RenderGui();

        ImGui::End();
    }

    return ret;
}

//----------------------------------------------------------------------------------------------------
bool MainMenuStateClient::Exit() const
{
    // Scene
    g_gameClient->GetEntityManager().ClearEntities();

    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    std::weak_ptr<State> emptyState = std::weak_ptr<State>();
    mainMenuComponent.m_fsm.ChangeState(emptyState);

    return true;
}

//----------------------------------------------------------------------------------------------------
void MainMenuStateClient::OnNotify(const Event& event)
{
    MainMenuComponent& mainMenuComponent = g_gameClient->GetMainMenuComponent();
    mainMenuComponent.m_fsm.OnNotify(event);
}
}
