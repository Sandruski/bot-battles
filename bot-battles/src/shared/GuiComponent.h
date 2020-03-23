#ifndef __GUI_COMPONENT_H__
#define __GUI_COMPONENT_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct GuiComponent {

    GuiComponent();

    bool m_isDebugOptions;
    std::function<void()> m_body;
};
}

#endif
