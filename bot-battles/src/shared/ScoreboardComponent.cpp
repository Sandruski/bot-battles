#include "ScoreboardComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
ScoreboardComponent::ScoreboardComponent()
    : m_mainMenuTimeout(15.0f)
    , m_timer()
    , m_phase(ScoreboardPhase::NONE)
{
}
}
