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

    std::weak_ptr<WindowComponent> windowComponent = g_game->GetWindowComponent();
    if (windowComponent.lock()->m_displayMode != WindowComponent::DisplayMode::FULLSCREEN) {
        const char* resolutions[] = { "896x504", "1280x720", "1920x1080" };
        I32 currentResolution = windowComponent.lock()->m_resolution;
        if (ImGui::Combo("Resolution", &currentResolution, resolutions, IM_ARRAYSIZE(resolutions))) {
            windowComponent.lock()->m_resolution = static_cast<WindowComponent::Resolution>(currentResolution);
            windowComponent.lock()->UpdateCurrentResolution();
        }
    }

    const char* displayModes[] = { "Fullscreen", "Windowed", "Borderless" };
    I32 currentDisplayMode = windowComponent.lock()->m_displayMode;
    if (ImGui::Combo("Display", &currentDisplayMode, displayModes, IM_ARRAYSIZE(displayModes))) {
        windowComponent.lock()->m_displayMode = static_cast<WindowComponent::DisplayMode>(currentDisplayMode);
        windowComponent.lock()->UpdateDisplayMode();
    }

    ImGui::InputFloat("FPS cap", &windowComponent.lock()->m_fps);

    ImGui::Spacing();

    std::weak_ptr<RendererComponent> rendererComponent = g_game->GetRendererComponent();
    if (ImGui::Checkbox("VSync", &rendererComponent.lock()->m_isVSync)) {
        rendererComponent.lock()->UpdateVSync();
    }

    return true;
}
}
