#ifndef __GUI_COMPONENT_H__
#define __GUI_COMPONENT_H__

namespace sand {

class Panel;

//----------------------------------------------------------------------------------------------------
// System Component
struct GuiComponent {

    GuiComponent();

    template <class T>
    bool RegisterPanel();
    template <class T>
    bool DeRegisterPanel();

    template <class T>
    bool SetCurrentPanel();

    std::array<std::shared_ptr<Panel>, MAX_PANELS> m_panels;
    std::weak_ptr<Panel> m_currentPanel;
    bool m_isDebugOptions;
};

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool GuiComponent::RegisterPanel()
{
    static_assert(std::is_base_of<Panel, T>::value, "T is not derived from Panel");

    PanelType panelType = T::GetType();
    assert(static_cast<U16>(panelType) < INVALID_PANEL);
    std::size_t panelIndex = static_cast<std::size_t>(panelType);
    std::shared_ptr<Panel> panel = m_panels.at(panelIndex);
    if (panel != nullptr) {
        WLOG("Panel %u is already registered!", panelIndex);
        return false;
    }

    panel = std::make_shared<T>();
    m_panels.at(panelIndex) = panel;

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool GuiComponent::DeRegisterPanel()
{
    static_assert(std::is_base_of<Panel, T>::value, "T is not derived from Panel");

    PanelType panelType = T::GetType();
    assert(static_cast<U16>(panelType) < INVALID_PANEL);
    std::size_t panelIndex = static_cast<std::size_t>(panelType);
    std::shared_ptr<Panel> panel = m_panels.at(panelIndex);
    if (panel == nullptr) {
        WLOG("Panel %u is not registered!", panelIndex);
        return false;
    }

    m_panels.at(panelIndex) = nullptr;

    return true;
}

//----------------------------------------------------------------------------------------------------
template <class T>
inline bool GuiComponent::SetCurrentPanel()
{
    static_assert(std::is_base_of<Panel, T>::value, "T is not derived from Panel");

    PanelType panelType = T::GetType();
    assert(static_cast<U16>(panelType) < INVALID_PANEL);
    std::size_t panelIndex = static_cast<std::size_t>(panelType);
    std::shared_ptr<Panel> panel = m_panels.at(panelIndex);
    if (panel == nullptr) {
        WLOG("Panel could not be set to %u!", panelIndex);
        return false;
    }

    m_currentPanel = std::weak_ptr<Panel>(panel);

    return !m_currentPanel.expired();
}
}

#endif
