#include "GraphicsPanel.h"

#include "Game.h"
#include "RendererComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string GraphicsPanel::GetName() const
{
    return "Graphics";
}

//----------------------------------------------------------------------------------------------------
bool GraphicsPanel::RenderHeader() const
{
    return ImGui::Button(GetName().c_str());
}

//----------------------------------------------------------------------------------------------------
bool GraphicsPanel::RenderBody() const
{
    ImGui::Text("%s", GetName().c_str());
    ImGui::Separator();
    ImGui::Spacing();

    RendererComponent& rendererComponent = g_game->GetRendererComponent();
    ImGui::Checkbox("Debug Draw", &rendererComponent.m_isDebugDraw);

    return true;
}
}
