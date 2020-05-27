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
    , m_color(White)
    , m_pct(0.0f)
    , m_isVisible(true)
{
}

//----------------------------------------------------------------------------------------------------
SpriteComponent::~SpriteComponent()
{
    g_game->GetResourceManager().RemoveResource<SpriteResource>(m_spriteResource.lock()->GetID());
}

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
    if (it != m_spriteNameToTextureCoords.end()) {
        textureCoords = it->second;
    } else if (!m_spriteResource.expired()) {
        glm::uvec2 size = m_spriteResource.lock()->GetSize();
        textureCoords = glm::uvec4(0, 0, size.x, size.y);
    } else {
        textureCoords = glm::uvec4(0, 0, 0, 0);
    }

    return textureCoords;
}

//----------------------------------------------------------------------------------------------------
const glm::uvec4 SpriteComponent::GetSpriteTextureCoords() const
{
    return GetSpriteTextureCoords(m_spriteName);
}
}
