#include "StatesPanel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
bool StatesPanel::RenderHeader() const
{
    return ImGui::Button("States");
}
}
