#include "NetworkingPanel.h"

#include "GameServer.h"
#include "ServerComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool NetworkingPanel::RenderBody() const
{
    ImGui::Text("%s", GetName().c_str());
    ImGui::Separator();
    ImGui::Spacing();

    ServerComponent& serverComponent = g_gameServer->GetServerComponent();
    ImGui::Checkbox("Server Rewind", &serverComponent.m_isServerRewind);

    return true;
}
}
