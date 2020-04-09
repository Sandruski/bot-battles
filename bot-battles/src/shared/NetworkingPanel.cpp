#include "NetworkingPanel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool NetworkingPanel::RenderHeader() const
{
    return ImGui::Button("Networking");
}
}
