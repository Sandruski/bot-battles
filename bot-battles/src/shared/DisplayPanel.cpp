#include "DisplayPanel.h"

#include "Game.h"
#include "WindowComponent.h"

namespace sand {

// TODO: move WindowPanel to the generic Settings panel and no the Debug Options panel
//----------------------------------------------------------------------------------------------------
bool DisplayPanel::RenderHeader() const
{
    return ImGui::Button("Display");
}

//----------------------------------------------------------------------------------------------------
bool DisplayPanel::RenderBody() const
{
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    const char* items[] = { "Fullscreen", "Windowed", "Borderless" };
    I32 currentItem = windowComponent.m_displayMode;
    if (ImGui::Combo("Window Mode", &currentItem, items, IM_ARRAYSIZE(items))) {
        windowComponent.m_displayMode = static_cast<WindowComponent::DisplayMode>(currentItem);
        windowComponent.UpdateDisplayMode();
    }

    return true;
}
}
