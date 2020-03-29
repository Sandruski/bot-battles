#include "ScoreboardComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ScoreboardComponent::ScoreboardComponent()
    :
#ifdef _CLIENT
    m_mainMenuTimeout(15.0f)
    , m_timer()
    ,
#endif
    m_phase(ScoreboardPhase::NONE)
    , m_gameCount(0)
{
}
}
