#ifndef __MAIN_MENU_COMPONENT_H__
#define __MAIN_MENU_COMPONENT_H__

#include "FSM.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct MainMenuComponent {

    MainMenuComponent();

    FSM m_fsm;
#ifdef _CLIENT
    MyTimer m_helloTimer;
    MyTimer m_guiTimer;
#endif
};
}

#endif
