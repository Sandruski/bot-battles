#include "WindowPanel.h"

#include "Game.h"
#include "WindowComponent.h"

namespace sand {

// TODO: move WindowPanel to the generic Settings panel and no the Debug Options panel
//----------------------------------------------------------------------------------------------------
bool WindowPanel::RenderHeader() const
{
    return ImGui::Button("Window");
}

//----------------------------------------------------------------------------------------------------
bool WindowPanel::RenderBody() const
{
    WindowComponent& windowComponent = g_game->GetWindowComponent();
    if (ImGui::Checkbox("Fullscreen", &windowComponent.m_isFullscreen)) {
        windowComponent.UpdateFullscreen();
    }

    return true;
}
}
