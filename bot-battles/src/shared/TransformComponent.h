#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "Component.h"

namespace sand {

struct InputComponent;

//----------------------------------------------------------------------------------------------------
// Entity Component
struct TransformComponent : public Component {

    static ComponentType GetType() { return ComponentType::TRANSFORM; }

    TransformComponent();

    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
    void Read(InputMemoryStream& inputStream, U32 dirtyState) override;

    void Move(const InputComponent& inputComponent, F32 dt);

    Vec3 m_position;
    F32 m_rotation;
};
}

#endif
