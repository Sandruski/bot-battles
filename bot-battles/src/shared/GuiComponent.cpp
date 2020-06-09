#include "GuiComponent.h"

#include "Panel.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GuiComponent::GuiComponent()
    : m_settingsPanels()
    , m_debugOptionsPanels()
    , m_currentSettingsPanel()
    , m_currentDebugOptionsPanel()
    , m_bigFont(nullptr)
    , m_secondsBetweenEllipsis(3.0f)
    , m_isSettings(false)
    , m_isDebugOptions(false)
{
    m_settingsPanels.fill(nullptr);
    m_debugOptionsPanels.fill(nullptr);
}
}
