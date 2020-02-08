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
    void Read(InputMemoryStream& inputStream, U32 dirtyState, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    bool AddSprite(std::string name, SDL_Rect rect);
    bool RemoveSprite(std::string name);
    const SDL_Rect& GetSprite(std::string name) const;
    const SDL_Rect& GetCurrentSprite() const;
    bool HasCurrentSprite() const;

    std::weak_ptr<SpriteResource> m_spriteResource;
    std::unordered_map<std::string, SDL_Rect> m_sprites;
    std::string m_currentSprite;
};
}

#endif
