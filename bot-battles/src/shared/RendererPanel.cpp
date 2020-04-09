#include "RendererPanel.h"

#include "Game.h"
#include "RendererComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool RendererPanel::RenderHeader() const
{
    return ImGui::Button("Graphics");
}

//----------------------------------------------------------------------------------------------------
bool RendererPanel::RenderBody() const
{
    RendererComponent& rendererComponent = g_game->GetRendererComponent();
    ImGui::Checkbox("Debug Draw", &rendererComponent.m_isDebugDraw);

    return true;
}
}
