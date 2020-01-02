#ifndef __SPRITE_COMPONENT_H__
#define __SPRITE_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif _SERVER
#include "NetworkableWriteObject.h"
#endif

namespace sand {

class SpriteResource;

//----------------------------------------------------------------------------------------------------
// Entity Component
struct SpriteComponent : public Component,
#ifdef _CLIENT
                         public NetworkableReadObject
#elif _SERVER
                         public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::SPRITE; }

    SpriteComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, ReplicationActionType replicationActionType, Entity entity) override;
#elif _SERVER
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    std::weak_ptr<SpriteResource> m_sprite;
};
}

#endif
