#include "BotComponent.h"

#include "ComponentMemberTypes.h"

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

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void BotComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::BOT_ACTION_TYPE)) {
        inputStream.Read(m_actionType);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::BOT_TIME_ACTION)) {
        inputStream.Read(m_timeAction);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U64 BotComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::BOT_ACTION_TYPE)) {
        outputStream.Write(m_actionType);
        writtenState |= static_cast<U64>(ComponentMemberType::BOT_ACTION_TYPE);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::BOT_TIME_ACTION)) {
        outputStream.Write(m_timeAction);
        writtenState |= static_cast<U64>(ComponentMemberType::BOT_TIME_ACTION);
    }

    return writtenState;
}
#endif

//----------------------------------------------------------------------------------------------------
bool BotComponent::CanPerformAction() const
{
    return m_actionType == BotComponent::ActionType::NONE;
}
}
