#include "DisplayPanel.h"

#include "Game.h"
#include "RendererComponent.h"
#include "WindowComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string DisplayPanel::GetName() const
{
    return "Display";
}

//----------------------------------------------------------------------------------------------------
bool DisplayPanel::RenderHeader() const
{
    return ImGui::Button(GetName().c_str());
}

//----------------------------------------------------------------------------------------------------
bool DisplayPanel::RenderBody() const
{
    ImGui::Text("%s", GetName().c_str());
    ImGui::Separator();
    ImGui::Spacing();

    WindowComponent& windowComponent = g_game->GetWindowComponent();
    const char* resolutions[] = { "1920x1080", "1280x720", "640x480" };
    I32 currentResolution = windowComponent.m_resolution;
    if (ImGui::Combo("Resolution", &currentResolution, resolutions, IM_ARRAYSIZE(resolutions))) {
        windowComponent.m_resolution = static_cast<WindowComponent::Resolution>(currentResolution);
        windowComponent.UpdateResolution();
    }

    const char* displayModes[] = { "Fullscreen", "Windowed", "Borderless" };
    I32 currentDisplayMode = windowComponent.m_displayMode;
    if (ImGui::Combo("Display Mode", &currentDisplayMode, displayModes, IM_ARRAYSIZE(displayModes))) {
        windowComponent.m_displayMode = static_cast<WindowComponent::DisplayMode>(currentDisplayMode);
        windowComponent.UpdateDisplayMode();
    }

    RendererComponent& rendererComponent = g_game->GetRendererComponent();
    if (ImGui::Checkbox("VSync", &rendererComponent.m_isVSync)) {
        rendererComponent.UpdateVSync();
    }

    return true;
}
}