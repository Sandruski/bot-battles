#include "SpriteComponent.h"

#include "ComponentMemberTypes.h"
#include "SpriteResource.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
U32 SpriteComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_FILE)) {
        std::string file = m_spriteResource.lock()->GetFile();
        outputStream.Write(file);
        writtenState |= static_cast<U32>(ComponentMemberType::SPRITE_FILE);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS)) {
        outputStream.Write(m_spriteNameToTextureCoords);
        writtenState |= static_cast<U32>(ComponentMemberType::SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_SPRITE_NAME)) {
        outputStream.Write(m_spriteName);
        writtenState |= static_cast<U32>(ComponentMemberType::SPRITE_SPRITE_NAME);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_VISIBLE)) {
        outputStream.Write(m_isVisible);
        writtenState |= static_cast<U32>(ComponentMemberType::SPRITE_VISIBLE);
    }

    return writtenState;
}
}
