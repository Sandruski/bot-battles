#include "NetworkingPanel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string NetworkingPanel::GetName() const
{
    return "Networking";
}

//----------------------------------------------------------------------------------------------------
bool NetworkingPanel::RenderHeader() const
{
    return ImGui::Button(GetName().c_str());
}
}
