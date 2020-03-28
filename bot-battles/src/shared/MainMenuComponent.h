#ifndef __MAIN_MENU_COMPONENT_H__
#define __MAIN_MENU_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct MainMenuComponent : public Component {

    enum class MainMenuPhase : U16 {

        SETUP,
        CONNECT,

        NONE
    };

    MainMenuComponent();

#ifdef _CLIENT
    MyTimer m_timer;
#endif
    MainMenuPhase m_phase;
};
}

#endif
