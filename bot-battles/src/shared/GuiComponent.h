#ifndef __GUI_COMPONENT_H__
#define __GUI_COMPONENT_H__

namespace sand {

class Panel;

//----------------------------------------------------------------------------------------------------
// System Component
struct GuiComponent {

    GuiComponent();

    template <class T>
    bool RegisterSettingsPanel();
    template <class T>
    bool RegisterDebugOptionsPanel();
    template <class T>
    bool DeRegisterSettingsPanel();
    template <class T>
    bool DeRegisterDebugOptionsPanel();

    template <class T>
    bool SetCurrentDebugOptionsPanel();
    template <class T>
    bool SetCurrentSettingsPanel();

    std::array<std::shared_ptr<Panel>, MAX_SETTINGS_PANELS> m_settingsPanels;
    std::array<std::shared_ptr<Panel>, MAX_DEBUG_OPTIONS_PANELS> m_debugOptionsPanels;
    std::weak_ptr<Panel> m_currentSettingsPanel;
    std::weak_ptr<Panel> m_currentDebugOptionsPanel;

    ImFont* m_bigFont;

    F32 m_secondsBetweenEllipsis;

    bool m_isSettings;
    bool m_isDebugOptions;
};

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool GuiComponent::RegisterSettingsPanel()
{
    static_assert(std::is_base_of<Panel, T>::value, "T is not derived from Panel");

    SettingsPanelType panelType = T::GetType();
    assert(static_cast<U8>(panelType) < INVALID_SETTINGS_PANEL);
    std::size_t panelIndex = static_cast<std::size_t>(panelType);
    std::shared_ptr<Panel> panel = m_settingsPanels.at(panelIndex);
    if (panel != nullptr) {
        WLOG("Panel %u is already registered!", panelIndex);
        return false;
    }

    panel = std::make_shared<T>();
    m_settingsPanels.at(panelIndex) = panel;

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool GuiComponent::RegisterDebugOptionsPanel()
{
    static_assert(std::is_base_of<Panel, T>::value, "T is not derived from Panel");

    DebugOptionsPanelType panelType = T::GetType();
    assert(static_cast<U8>(panelType) < INVALID_DEBUG_OPTIONS_PANEL);
    std::size_t panelIndex = static_cast<std::size_t>(panelType);
    std::shared_ptr<Panel> panel = m_debugOptionsPanels.at(panelIndex);
    if (panel != nullptr) {
        WLOG("Panel %u is already registered!", panelIndex);
        return false;
    }

    panel = std::make_shared<T>();
    m_debugOptionsPanels.at(panelIndex) = panel;

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool GuiComponent::DeRegisterSettingsPanel()
{
    static_assert(std::is_base_of<Panel, T>::value, "T is not derived from Panel");

    SettingsPanelType panelType = T::GetType();
    assert(static_cast<U8>(panelType) < INVALID_SETTINGS_PANEL);
    std::size_t panelIndex = static_cast<std::size_t>(panelType);
    std::shared_ptr<Panel> panel = m_settingsPanels.at(panelIndex);
    if (panel == nullptr) {
        WLOG("Panel %u is not registered!", panelIndex);
        return false;
    }

    m_settingsPanels.at(panelIndex) = nullptr;

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool GuiComponent::DeRegisterDebugOptionsPanel()
{
    static_assert(std::is_base_of<Panel, T>::value, "T is not derived from Panel");

    DebugOptionsPanelType panelType = T::GetType();
    assert(static_cast<U8>(panelType) < INVALID_DEBUG_OPTIONS_PANEL);
    std::size_t panelIndex = static_cast<std::size_t>(panelType);
    std::shared_ptr<Panel> panel = m_debugOptionsPanels.at(panelIndex);
    if (panel == nullptr) {
        WLOG("Panel %u is not registered!", panelIndex);
        return false;
    }

    m_debugOptionsPanels.at(panelIndex) = nullptr;

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool GuiComponent::SetCurrentSettingsPanel()
{
    static_assert(std::is_base_of<Panel, T>::value, "T is not derived from Panel");

    SettingsPanelType panelType = T::GetType();
    assert(static_cast<U8>(panelType) < INVALID_SETTINGS_PANEL);
    std::size_t panelIndex = static_cast<std::size_t>(panelType);
    std::shared_ptr<Panel> panel = m_settingsPanels.at(panelIndex);
    if (panel == nullptr) {
        WLOG("Panel could not be set to %u!", panelIndex);
        return false;
    }

    m_currentSettingsPanel = std::weak_ptr<Panel>(panel);

    return !m_currentSettingsPanel.expired();
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool GuiComponent::SetCurrentDebugOptionsPanel()
{
    static_assert(std::is_base_of<Panel, T>::value, "T is not derived from Panel");

    DebugOptionsPanelType panelType = T::GetType();
    assert(static_cast<U8>(panelType) < INVALID_DEBUG_OPTIONS_PANEL);
    std::size_t panelIndex = static_cast<std::size_t>(panelType);
    std::shared_ptr<Panel> panel = m_debugOptionsPanels.at(panelIndex);
    if (panel == nullptr) {
        WLOG("Panel could not be set to %u!", panelIndex);
        return false;
    }

    m_currentDebugOptionsPanel = std::weak_ptr<Panel>(panel);

    return !m_currentDebugOptionsPanel.expired();
}
}

#endif
