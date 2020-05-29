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

    U64 Write(OutputMemoryStream& outputStream, U64 dirtyState) const override;
    void Read(InputMemoryStream& inputStream, U64 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;

    void Reset();
    void FullReset();

    // Bot
    F32 GetMaxLinearVelocity() const;
    F32 GetMaxAngularVelocity() const;

    void SetLinearVelocityX(F32 linearVelocityX);
    void SetLinearVelocityY(F32 linearVelocityY);
    F32 GetLinearVelocityX() const;
    F32 GetLinearVelocityY() const;
    void SetAngularVelocity(F32 angularVelocity);
    F32 GetAngularVelocity() const;
    void ShootPrimaryWeapon();
    void ShootSecondaryWeapon();

    F32 m_maxLinearVelocity; // pixels
    F32 m_maxAngularVelocity; // degrees

    glm::vec2 m_linearVelocity; // pixels
    F32 m_angularVelocity; // degrees

    U64 m_dirtyState;
};
}

#endif
