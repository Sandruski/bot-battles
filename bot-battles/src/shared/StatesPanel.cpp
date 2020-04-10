#include "StatesPanel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
std::string StatesPanel::GetName() const
{
    return "States";
}

//----------------------------------------------------------------------------------------------------
bool StatesPanel::RenderHeader() const
{
    return ImGui::Button(GetName().c_str());
}
}
