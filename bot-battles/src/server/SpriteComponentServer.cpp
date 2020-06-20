#include "SpriteComponent.h"

#include "ComponentMemberTypes.h"
#include "SpriteResource.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
U64 SpriteComponent::Write(OutputMemoryStream& outputStream, U64 dirtyState) const
{
    U64 writtenState = 0;

    if (dirtyState & static_cast<U64>(ComponentMemberType::SPRITE_FILE)) {
        std::string file = m_spriteResource.lock()->GetFile();
        outputStream.Write(file);
        writtenState |= static_cast<U64>(ComponentMemberType::SPRITE_FILE);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS)) {
        outputStream.Write(m_spriteNameToTextureCoords);
        writtenState |= static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS);
    }
    if (dirtyState & static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME)) {
        outputStream.Write(m_spriteName);
        writtenState |= static_cast<U64>(ComponentMemberType::SPRITE_SPRITE_NAME);
    }

    return writtenState;
}
}
