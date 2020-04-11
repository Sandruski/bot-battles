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
void LabelComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::LABEL_ENABLED)) {
        inputStream.Read(m_isEnabled);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::LABEL_OFFSET)) {
        inputStream.Read(m_offset);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::LABEL_TEXT)) {
        inputStream.Read(m_text);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::LABEL_COLOR)) {
        inputStream.Read(m_color);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 LabelComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::LABEL_ENABLED)) {
        outputStream.Write(m_isEnabled);
        writtenState |= static_cast<U32>(ComponentMemberType::LABEL_ENABLED);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::LABEL_OFFSET)) {
        outputStream.Write(m_offset);
        writtenState |= static_cast<U32>(ComponentMemberType::LABEL_OFFSET);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::LABEL_TEXT)) {
        outputStream.Write(m_text);
        writtenState |= static_cast<U32>(ComponentMemberType::LABEL_TEXT);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::LABEL_COLOR)) {
        outputStream.Write(m_color);
        writtenState |= static_cast<U32>(ComponentMemberType::LABEL_COLOR);
    }

    return writtenState;
}
#endif
}
