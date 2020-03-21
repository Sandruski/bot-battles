#include "SpriteComponent.h"

#include "ComponentMemberTypes.h"
#include "Game.h"
#include "ResourceManager.h"
#include "SpriteResource.h"

namespace sand {
//----------------------------------------------------------------------------------------------------
SpriteComponent::SpriteComponent()
    : m_spriteResource()
    , m_spriteNameToTextureCoords()
    , m_spriteName("default")
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
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS)) {
        inputStream.Read(m_spriteNameToTextureCoords);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_SPRITE_NAME)) {
        inputStream.Read(m_spriteName);
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
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS)) {
        outputStream.Write(m_spriteNameToTextureCoords);
        writtenState |= static_cast<U32>(ComponentMemberType::SPRITE_SPRITE_NAME_TO_TEXTURE_COORDS);
    }
    if (dirtyState & static_cast<U32>(ComponentMemberType::SPRITE_SPRITE_NAME)) {
        outputStream.Write(m_spriteName);
        writtenState |= static_cast<U32>(ComponentMemberType::SPRITE_SPRITE_NAME);
    }

    return writtenState;
}
#endif

//----------------------------------------------------------------------------------------------------
bool SpriteComponent::AddSprite(const std::string& name, const glm::uvec4& textureCoords)
{
    return m_spriteNameToTextureCoords.insert(std::make_pair(name, textureCoords)).second;
}

//----------------------------------------------------------------------------------------------------
bool SpriteComponent::RemoveSprite(const std::string& name)
{
    return m_spriteNameToTextureCoords.erase(name);
}

//----------------------------------------------------------------------------------------------------
const glm::uvec4 SpriteComponent::GetSpriteTextureCoords(const std::string& name) const
{
    glm::uvec4 textureCoords = glm::uvec4(0, 0, 0, 0);

    auto it = m_spriteNameToTextureCoords.find(name);
    if (it == m_spriteNameToTextureCoords.end()) {
        glm::uvec2 size = m_spriteResource.lock()->GetSize();
        textureCoords = glm::uvec4(0, 0, size.x, size.y);
    } else {
        textureCoords = it->second;
    }

    return textureCoords;
}

//----------------------------------------------------------------------------------------------------
const glm::uvec4 SpriteComponent::GetSpriteTextureCoords() const
{
    return GetSpriteTextureCoords(m_spriteName);
}
}
