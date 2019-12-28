#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "Component.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct TransformComponent : public Component {

    static ComponentType GetType() { return ComponentType::TRANSFORM; }
    static std::shared_ptr<TransformComponent> Instantiate() { return std::make_shared<TransformComponent>(); }

    TransformComponent();

    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
    void Read(InputMemoryStream& inputStream, U32 dirtyState) override;

    Vec3 m_position;
    float m_rotation;
};
}

#endif
