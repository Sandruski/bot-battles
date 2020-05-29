#include "LabelComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
LabelComponent::LabelComponent()
    : m_offset(0.0f, 0.0f)
    , m_text()
    , m_color(0.0f, 0.0f, 0.0f, 0.0f)
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void LabelComponent::Read(InputMemoryStream& inputStream, U64 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U64>(ComponentMemberType::LABEL_OFFSET)) {
        inputStream.Read(m_offset);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::LABEL_TEXT)) {
        inputStream.Read(m_text);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::LABEL_COLOR)) {
        inputStream.Read(m_color);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U64 LabelComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::LABEL_OFFSET)) {
        outputStream.Write(m_offset);
        writtenState |= static_cast<U64>(ComponentMemberType::LABEL_OFFSET);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::LABEL_TEXT)) {
        outputStream.Write(m_text);
        writtenState |= static_cast<U64>(ComponentMemberType::LABEL_TEXT);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::LABEL_COLOR)) {
        outputStream.Write(m_color);
        writtenState |= static_cast<U64>(ComponentMemberType::LABEL_COLOR);
    }

    return writtenState;
}
#endif
}
