#include "BotComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
BotComponent::BotComponent()
    : m_actionType(ActionType::NONE)
    , m_timeAction(0.0f)
#ifdef _SERVER
    , m_cooldownAction(0.0f)
    , m_timerAction()
#endif
{
}

//----------------------------------------------------------------------------------------------------
bool BotComponent::CanPerformAction() const
{
    return m_actionType == BotComponent::ActionType::NONE;
}
}
