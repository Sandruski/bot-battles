#include "MainMenuComponent.h"

namespace sand {

// TODO: fill from config with main menu info (e.g. timers)

//----------------------------------------------------------------------------------------------------
MainMenuComponent::MainMenuComponent()
    : m_fsm()
#ifdef _CLIENT
    , m_helloTimer()
    , m_guiTimer()
#endif
{
}
}
