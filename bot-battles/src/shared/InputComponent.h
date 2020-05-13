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

    F32 GetMaxLinearVelocity() const;
    F32 GetMaxAngularVelocity() const;

    void SetLinearVelocity(const glm::vec2& linearVelocity);
    glm::vec2 GetLinearVelocity() const;
    void SetAngularVelocity(F32 angularVelocity);
    F32 GetAngularVelocity() const;
    void Shoot();

    F32 m_maxLinearVelocity; // pixels
    F32 m_maxAngularVelocity; // degrees

    glm::vec2 m_linearVelocity; // pixels
    F32 m_angularVelocity; // degrees

    U32 m_dirtyState;
};
}

#endif
