#include "MainMenuComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
MainMenuComponent::MainMenuComponent()
    :
#ifdef _CLIENT
    m_timer()
    ,
#endif
    m_phase(MainMenuPhase::NONE)
{
}
}
