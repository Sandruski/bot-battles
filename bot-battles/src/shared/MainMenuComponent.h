#ifndef __MAIN_MENU_COMPONENT_H__
#define __MAIN_MENU_COMPONENT_H__

#include "FSM.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct MainMenuComponent {

    MainMenuComponent();

    FSM m_fsm;
    Entity m_author;
    Entity m_version;
    std::pair<std::string, LogTypes> m_log;
#ifdef _CLIENT
    MyTimer m_helloTimer;
    MyTimer m_guiTimer;
    F32 m_secondsBetweenHello;
#endif
};
}

#endif
