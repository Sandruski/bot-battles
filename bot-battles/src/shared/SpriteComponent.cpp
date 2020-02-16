#include "SpriteComponent.h"

#include "ComponentMemberTypes.h"
#include "Game.h"
#include "ResourceManager.h"
#include "SpriteResource.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
SpriteComponent::SpriteComponent()
    : m_spriteResource()
    , m_sprites()
    , m_currentSprite()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void SpriteComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_FILE)) {
        std::string file;
        inputStream.Read(file);
        m_spriteResource = g_game->GetResourceManager().AddResource<SpriteResource>(file.c_str(), TEXTURES_DIR, true);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_SPRITES)) {
        inputStream.Read(m_sprites);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_CURRENT_SPRITE)) {
        inputStream.Read(m_currentSprite);
    }
}
#elif defined(_SERVER)
//----------------------------------------------------------------------------------------------------
U32 SpriteComponent::Write(OutputMemoryStream& outputStream, U32 dirtyState) const
{
    U32 writtenState = 0;

    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_FILE)) {
        std::string file = m_spriteResource.lock()->GetFile();
        outputStream.Write(file);
        writtenState |= static_cast<U32>(ComponentMemberType::SPRITE_FILE);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_SPRITES)) {
        outputStream.Write(m_sprites);
        writtenState |= static_cast<U32>(ComponentMemberType::SPRITE_SPRITES);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_CURRENT_SPRITE)) {
        outputStream.Write(m_currentSprite);
        writtenState |= static_cast<U32>(ComponentMemberType::SPRITE_CURRENT_SPRITE);
    }

    return writtenState;
}
#endif

//----------------------------------------------------------------------------------------------------
bool SpriteComponent::AddSprite(std::string name, SDL_Rect rect)
{
    return m_sprites.insert(std::make_pair(name, rect)).second;
}

//----------------------------------------------------------------------------------------------------
bool SpriteComponent::RemoveSprite(std::string name)
{
    return m_sprites.erase(name);
}

//----------------------------------------------------------------------------------------------------
const SDL_Rect& SpriteComponent::GetSprite(std::string name) const
{
    return m_sprites.at(name);
}

//----------------------------------------------------------------------------------------------------
const SDL_Rect& SpriteComponent::GetCurrentSprite() const
{
    return m_sprites.at(m_currentSprite);
}

//----------------------------------------------------------------------------------------------------
bool SpriteComponent::HasCurrentSprite() const
{
    return !m_currentSprite.empty();
}
}
