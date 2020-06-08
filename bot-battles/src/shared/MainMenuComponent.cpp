#include "MainMenuComponent.h"

namespace sand {

// TODO: fill from config with main menu info (e.g. timers)

//----------------------------------------------------------------------------------------------------
MainMenuComponent::MainMenuComponent()
    : m_fsm()
    , m_author()
    , m_version()
    , m_log()
#ifdef _CLIENT
    , m_helloTimer()
    , m_guiTimer()
    , m_secondsBetweenHello(1.0f)
#endif
{
}
}
