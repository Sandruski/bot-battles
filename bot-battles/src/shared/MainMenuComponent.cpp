#include "MainMenuComponent.h"

namespace sand {

// TODO: fill from config with gameplay info

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
