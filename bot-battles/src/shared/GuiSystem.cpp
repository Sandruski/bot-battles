#include "GuiSystem.h"

#include "Game.h"
#include "GuiComponent.h"
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

    WindowComponent windowComponent = g_game->GetWindowComponent();
    ImGui_ImplSDL2_InitForOpenGL(windowComponent.m_window, SDL_GL_GetCurrentContext());
    const char* glslVersion = "#version 130"; // TODO: store in a struct
    ImGui_ImplOpenGL3_Init(glslVersion);

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GuiSystem::PreRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    WindowComponent windowComponent = g_game->GetWindowComponent();
    ImGui_ImplSDL2_NewFrame(windowComponent.m_window);
    ImGui::NewFrame();

    GuiComponent guiComponent = g_game->GetGuiComponent();
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoTitleBar;
    windowFlags |= ImGuiWindowFlags_NoScrollbar;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_MenuBar;

    if (!ImGui::Begin("Debug Options", &guiComponent.m_isOpen, windowFlags)) {
        ImGui::End();
        return true;
    }

    if (!ImGui::BeginMenuBar()) {
        guiComponent.m_isOpen = false;
        ImGui::EndMenuBar();
        ImGui::End();
    }

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GuiSystem::Render()
{
    GuiComponent guiComponent = g_game->GetGuiComponent();
    if (guiComponent.m_isOpen) {
        ImGui::EndMenuBar();

        if (guiComponent.m_body) {
            guiComponent.m_body();
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return true;
}

//----------------------------------------------------------------------------------------------------
bool GuiSystem::PostRender()
{
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
