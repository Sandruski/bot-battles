#ifndef __GUI_COMPONENT_H__
#define __GUI_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct GuiComponent {

    GuiComponent();

    bool m_isOpen;
    std::function<void()> m_body;
};
}

#endif
