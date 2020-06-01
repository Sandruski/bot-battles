#include "BotComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
BotComponent::BotComponent()
    : m_canPerformAction(true)
    , m_canPerformAnimation(true)
    , m_timeAction(0.0f)
    , m_cooldownAction(0.0f)
    , m_timerAction()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void BotComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::BOT_CAN_PERFORM_ACTION)) {
        inputStream.Read(m_canPerformAction);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U64 BotComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::BOT_CAN_PERFORM_ACTION)) {
        outputStream.Write(m_canPerformAction);
        writtenState |= static_cast<U64>(ComponentMemberType::BOT_CAN_PERFORM_ACTION);
    }

    return writtenState;
}
#endif
}
