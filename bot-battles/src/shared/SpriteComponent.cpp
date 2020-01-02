#include "SpriteComponent.h"

#include "ComponentMemberTypes.h"
#include "Game.h"
#include "ResourceManager.h"
#include "SpriteResource.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
SpriteComponent::SpriteComponent()
    : m_sprite()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void SpriteComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_FILE)) {
        std::string file;
        inputStream.Read(file);
        m_sprite = g_game->GetResourceManager().AddResource<SpriteResource>(file.c_str(), TEXTURES_DIR, true);
    }
}
#elif _SERVER
//----------------------------------------------------------------------------------------------------
U32 SpriteComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_FILE)) {
        std::string file = m_sprite.lock()->GetFile();
        outputStream.Write(file);
        writtenState |= static_cast<U32>(ComponentMemberType::SPRITE_FILE);
    }

    return writtenState;
}
#endif
}
