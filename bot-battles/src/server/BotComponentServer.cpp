#include "BotComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

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
}
