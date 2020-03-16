#include "SpriteComponent.h"

#include "ComponentMemberTypes.h"
#include "Game.h"
#include "ResourceManager.h"
#include "SpriteResource.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
SpriteComponent::SpriteComponent()
    : m_spriteResource()
    , m_spriteToTextureCoords()
    , m_currentSprite()
{
}

#ifdef _CLIENT
//----------------------------------------------------------------------------------------------------
void SpriteComponent::Read(InputMemoryStream& inputStream, U32 dirtyState, U32 /*frame*/, ReplicationActionType /*replicationActionType*/, Entity /*entity*/)
{
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_ENABLED)) {
        inputStream.Read(m_isEnabled);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_FILE)) {
        std::string file;
        inputStream.Read(file);
        m_spriteResource = g_game->GetResourceManager().AddResource<SpriteResource>(file.c_str(), TEXTURES_DIR, true);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_SPRITES)) {
        inputStream.Read(m_spriteToTextureCoords);
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

    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_ENABLED)) {
        outputStream.Write(m_isEnabled);
        writtenState |= static_cast<U32>(ComponentMemberType::SPRITE_ENABLED);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_FILE)) {
        std::string file = m_spriteResource.lock()->GetFile();
        outputStream.Write(file);
        writtenState |= static_cast<U32>(ComponentMemberType::SPRITE_FILE);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_SPRITES)) {
        outputStream.Write(m_spriteToTextureCoords);
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
bool SpriteComponent::AddSprite(const std::string& name, const glm::vec4& textureCoords)
{
    return m_spriteToTextureCoords.insert(std::make_pair(name, textureCoords)).second;
}

//----------------------------------------------------------------------------------------------------
bool SpriteComponent::RemoveSprite(const std::string& name)
{
    return m_spriteToTextureCoords.erase(name);
}

//----------------------------------------------------------------------------------------------------
bool SpriteComponent::GetSprite(const std::string& name, glm::vec4& textureCoords) const
{
    auto it = m_spriteToTextureCoords.find(name);
    if (it == m_spriteToTextureCoords.end()) {
        // TODO: change this to force to have a current sprite
        glm::uvec2 size = !m_spriteResource.expired() ? m_spriteResource.lock()->GetSize() : glm::uvec2(1.0f, 1.0f);
        textureCoords = glm::vec4(0.0f, 0.0f, size.x, size.y);

        return false;
    }

    textureCoords = it->second;

    return true;
}

//----------------------------------------------------------------------------------------------------
bool SpriteComponent::GetCurrentSprite(glm::vec4& textureCoords) const
{
    return GetSprite(m_currentSprite, textureCoords);
}
}
