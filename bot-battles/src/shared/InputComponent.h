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

#ifdef _CLIENT
    // Python
    void SetLinearVelocityX(F32 linearVelocityX);

    F32 GetLinearVelocityX() const
    {
        return m_linearVelocity.x;
    }

    void SetLinearVelocityY(F32 linearVelocityY);

    F32 GetLinearVelocityY() const
    {
        return m_linearVelocity.y;
    }

    void SetAngularVelocity(F32 angularVelocity);

    F32 GetAngularVelocity() const
    {
        return m_angularVelocity;
    }

    void ShootPrimaryWeapon();
    void ShootSecondaryWeapon();
    void Reload();
    void Heal();
#endif

    glm::vec2 m_linearVelocity;
    F32 m_angularVelocity;

    U64 m_dirtyState;
};
}

#endif
