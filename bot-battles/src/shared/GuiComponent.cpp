#include "GuiComponent.h"

#include "Panel.h"

namespace sand {

// TODO: with config true/false

//----------------------------------------------------------------------------------------------------
GuiComponent::GuiComponent()
    : m_settingsPanels()
    , m_debugOptionsPanels()
    , m_currentSettingsPanel()
    , m_currentDebugOptionsPanel()
    , m_isSettings(true)
    , m_isDebugOptions(true)
{
    m_settingsPanels.fill(nullptr);
    m_debugOptionsPanels.fill(nullptr);
}
}
