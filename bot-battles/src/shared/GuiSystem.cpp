#include "GuiSystem.h"

#include "EventComponent.h"
#include "Game.h"
#include "GuiComponent.h"
#include "Panel.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GuiSystem::GuiSystem()
{
}

//----------------------------------------------------------------------------------------------------
bool GuiSystem::StartUp()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    WindowComponent& windowComponent = g_game->GetWindowComponent();
    ImGui_ImplSDL2_InitForOpenGL(windowComponent.m_window, SDL_GL_GetCurrentContext());
    const char* glslVersion = "#version 130";
    ImGui_ImplOpenGL3_Init(glslVersion);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GuiSystem::Update()
{
    EventComponent& eventComponent = g_game->GetEventComponent();
    if (eventComponent.m_keyboard.at(SDL_SCANCODE_F1) == EventComponent::KeyState::DOWN) {
        GuiComponent& guiComponent = g_game->GetGuiComponent();
        guiComponent.m_isDebugOptions = !guiComponent.m_isDebugOptions;
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GuiSystem::PreRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    ImGui_ImplSDL2_NewFrame(windowComponent.m_window);
    ImGui::NewFrame();

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GuiSystem::RenderGui()
{
    GuiComponent& guiComponent = g_game->GetGuiComponent();
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoTitleBar;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_MenuBar;
    if (ImGui::Begin("Debug Options", &guiComponent.m_isDebugOptions, windowFlags)) {
        if (ImGui::BeginMenuBar()) {
            for (const auto& panel : guiComponent.m_panels) {
                if (panel != nullptr && panel->m_isEnabled) {
                    const bool result = panel->RenderHeader();
                    if (result) {
                        guiComponent.m_currentPanel = std::weak_ptr<Panel>(panel);
                    }
                }
            }

            ImGui::EndMenuBar();
        }

        if (!guiComponent.m_currentPanel.expired()) {
            guiComponent.m_currentPanel.lock()->RenderBody();
        }

        ImGui::End();
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
