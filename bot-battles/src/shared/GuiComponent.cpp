#include "GuiComponent.h"

#include "Panel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GuiComponent::GuiComponent()
    : m_panels()
    , m_currentPanel()
    , m_isDebugOptions(true)
{
    m_panels.fill(nullptr);
}
}
