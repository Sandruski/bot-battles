#include "LabelComponent.h"

#include "ComponentMemberTypes.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
LabelComponent::LabelComponent()
    : m_offset()
    , m_text()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void LabelComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::LABEL_OFFSET)) {
        inputStream.Read(m_offset);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::LABEL_TEXT)) {
        inputStream.Read(m_text);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 LabelComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::LABEL_OFFSET)) {
        outputStream.Write(m_offset);
        writtenState |= static_cast<U32>(ComponentMemberType::LABEL_OFFSET);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::LABEL_TEXT)) {
        outputStream.Write(m_text);
        writtenState |= static_cast<U32>(ComponentMemberType::LABEL_TEXT);
    }

    return writtenState;
}
#endif
}
