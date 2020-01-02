#ifndef __TEXT_COMPONENT_H__
#define __TEXT_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif _SERVER
#include "NetworkableWriteObject.h"
#endif

namespace sand {

class TextResource;

//----------------------------------------------------------------------------------------------------
// Entity Component
struct TextComponent : public Component,
#ifdef _CLIENT
                       public NetworkableReadObject
#elif _SERVER
                       public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::TEXT; }

    TextComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, bool isLocalPlayer) override;
#elif _SERVER
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    std::weak_ptr<TextResource> m_text;
};
}

#endif
