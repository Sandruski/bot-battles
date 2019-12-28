#ifndef __INPUT_COMPONENT_H__
#define __INPUT_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct InputComponent : public Component {

    static ComponentType GetType() { return ComponentType::INPUT; }
    static std::shared_ptr<InputComponent> Instantiate() { return std::make_shared<InputComponent>(); }

    InputComponent();

    void Copy(const InputComponent& other);

    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
    void Read(InputMemoryStream& inputStream, U32 dirtyState) override;

    Vec2 m_acceleration;
    float m_angularAcceleration;
};
}

#endif
