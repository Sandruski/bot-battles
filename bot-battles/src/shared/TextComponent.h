#ifndef __TEXT_COMPONENT_H__
#define __TEXT_COMPONENT_H__

#include "Component.h"
#include "NetworkableObject.h"
#include "TextResource.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct TextComponent : public Component, public NetworkableObject {

    static ComponentType GetType() { return ComponentType::TEXT; }
    static std::shared_ptr<TextComponent> Instantiate() { return std::make_shared<TextComponent>(); }

    TextComponent();
    ~TextComponent() override = default;

    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
    void Read(InputMemoryStream& inputStream, U32 dirtyState) override;

    std::shared_ptr<TextResource> m_text;
};
}

#endif
