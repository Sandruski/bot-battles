#ifndef __INPUT_COMPONENT_H__
#define __INPUT_COMPONENT_H__

#include "Component.h"
#include "NetworkableReadObject.h"
#include "NetworkableWriteObject.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// System Component
struct InputComponent : public Component,
                        public NetworkableWriteObject,
                        public NetworkableReadObject {
    InputComponent();

    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
    void Read(InputMemoryStream& inputStream, U32 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;

    void Shoot();

    // TODO: replace acceleration for velocity :)

    F32 m_maxLinearVelocity; // pixels
    F32 m_maxAngularVelocity; // degrees

    glm::vec2 m_linearVelocity; // pixels
    F32 m_angularVelocity; // degrees
    bool m_isShooting;
};
}

#endif
