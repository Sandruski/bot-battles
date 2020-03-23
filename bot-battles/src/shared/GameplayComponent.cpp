#include "GameplayComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
GameplayComponent::GameplayComponent()
    : m_timer()
    , m_phase(GameplayPhase::NONE)
{
}
}
