#include "GuiSystem.h"

#include "EventComponent.h"
#include "Game.h"
#include "GuiComponent.h"
#include "Panel.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool GuiSystem::StartUp()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    ImGui_ImplSDL2_InitForOpenGL(windowComponent.lock()->m_window, SDL_GL_GetCurrentContext());
    const char* glslVersion = "#version 130";
    ImGui_ImplOpenGL3_Init(glslVersion);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(windowComponent.lock()->m_window);
    ImGui::NewFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GuiSystem::Update()
{
    std::weak_ptr<EventComponent> eventComponent = g_game->GetEventComponent();
    if (eventComponent.lock()->m_keyboard.at(SDL_SCANCODE_ESCAPE) == EventComponent::KeyState::DOWN) {
        std::weak_ptr<GuiComponent> guiComponent = g_game->GetGuiComponent();
        guiComponent.lock()->m_isSettings = !guiComponent.lock()->m_isSettings;
    }
    if (eventComponent.lock()->m_keyboard.at(SDL_SCANCODE_F1) == EventComponent::KeyState::DOWN) {
        std::weak_ptr<GuiComponent> guiComponent = g_game->GetGuiComponent();
        guiComponent.lock()->m_isDebugOptions = !guiComponent.lock()->m_isDebugOptions;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GuiSystem::PreRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    ImGui_ImplSDL2_NewFrame(windowComponent.lock()->m_window);
    ImGui::NewFrame();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GuiSystem::RenderGui()
{
    OPTICK_EVENT();

    std::weak_ptr<GuiComponent> guiComponent = g_game->GetGuiComponent();
    if (guiComponent.lock()->m_isSettings) {
        ImGuiWindowFlags windowFlags = 0;
        windowFlags |= ImGuiWindowFlags_NoResize;
        windowFlags |= ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoScrollbar;
        windowFlags |= ImGuiWindowFlags_NoCollapse;
        windowFlags |= ImGuiWindowFlags_NoSavedSettings;
        windowFlags |= ImGuiWindowFlags_MenuBar;

        std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
        ImVec2 position = ImVec2(static_cast<F32>(windowComponent.lock()->m_currentResolution.x) / 2.0f, static_cast<F32>(windowComponent.lock()->m_currentResolution.y) / 2.0f);
        ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImVec2 size = ImVec2(static_cast<F32>(windowComponent.lock()->m_currentResolution.y) / 1.8f, static_cast<F32>(windowComponent.lock()->m_currentResolution.x) / 1.8f);
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);

        // TODO: "Options", "Credits", "Quit to Desktop" only when no match is running
        if (ImGui::Begin("Settings", &guiComponent.lock()->m_isSettings, windowFlags)) {
            if (ImGui::BeginMenuBar()) {
                for (const auto& panel : guiComponent.lock()->m_settingsPanels) {
                    if (panel != nullptr && panel->m_isEnabled) {
                        const bool result = panel->RenderHeader();
                        if (result) {
                            guiComponent.lock()->m_currentSettingsPanel = std::weak_ptr<Panel>(panel);
                        }
                    }
                }

                ImGui::EndMenuBar();
            }

            if (!guiComponent.lock()->m_currentSettingsPanel.expired()) {
                guiComponent.lock()->m_currentSettingsPanel.lock()->RenderBody();
            }

            ImGui::End();
        }
    }

    if (guiComponent.lock()->m_isDebugOptions) {
        ImGuiWindowFlags windowFlags = 0;
        windowFlags |= ImGuiWindowFlags_NoTitleBar;
        windowFlags |= ImGuiWindowFlags_NoCollapse;
        windowFlags |= ImGuiWindowFlags_MenuBar;
        windowFlags |= ImGuiWindowFlags_NoSavedSettings;

        if (ImGui::Begin("Debug Options", &guiComponent.lock()->m_isDebugOptions, windowFlags)) {
            if (ImGui::BeginMenuBar()) {
                for (const auto& panel : guiComponent.lock()->m_debugOptionsPanels) {
                    if (panel != nullptr && panel->m_isEnabled) {
                        const bool result = panel->RenderHeader();
                        if (result) {
                            guiComponent.lock()->m_currentDebugOptionsPanel = std::weak_ptr<Panel>(panel);
                        }
                    }
                }

                ImGui::EndMenuBar();
            }

            if (!guiComponent.lock()->m_currentDebugOptionsPanel.expired()) {
                guiComponent.lock()->m_currentDebugOptionsPanel.lock()->RenderBody();
            }

            ImGui::End();
        }
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GuiSystem::ShutDown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return true;
}
}
