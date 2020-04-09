#include "GraphicsPanel.h"

#include "Game.h"
#include "RendererComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool GraphicsPanel::RenderHeader() const
{
    return ImGui::Button("Graphics");
}

//----------------------------------------------------------------------------------------------------
bool GraphicsPanel::RenderBody() const
{
    RendererComponent& rendererComponent = g_game->GetRendererComponent();
    ImGui::Checkbox("Debug Draw", &rendererComponent.m_isDebugDraw);

    return true;
}
}
