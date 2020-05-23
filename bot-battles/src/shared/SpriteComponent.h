#ifndef __SPRITE_COMPONENT_H__
#define __SPRITE_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

class SpriteResource;

//----------------------------------------------------------------------------------------------------
// Entity Component
struct SpriteComponent : public Component
#ifdef _CLIENT
    ,
                         public NetworkableReadObject
#elif defined(_SERVER)
    ,
                         public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::SPRITE; }

    SpriteComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    bool AddSprite(const std::string& name, const glm::uvec4& textureCoords);
    bool RemoveSprite(const std::string& name);
    const glm::uvec4 GetSpriteTextureCoords(const std::string& name) const;
    const glm::uvec4 GetSpriteTextureCoords() const;

    // Networked
    std::weak_ptr<SpriteResource> m_spriteResource;
    std::unordered_map<std::string, glm::uvec4> m_spriteNameToTextureCoords;
    std::string m_spriteName;

    // Local
    glm::vec4 m_color;
    F32 m_pct;
};
}

#endif
