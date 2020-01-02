#ifndef __INPUT_COMPONENT_H__
#define __INPUT_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableWriteObject.h"
#elif _SERVER
#include "NetworkableReadObject.h"
#endif

namespace sand {

//----------------------------------------------------------------------------------------------------
// Entity Component
struct InputComponent : public Component,
#ifdef _CLIENT
                        public NetworkableWriteObject
#elif _SERVER
                        public NetworkableReadObject
#endif
{
    static ComponentType GetType() { return ComponentType::INPUT; }

    InputComponent();

    void Copy(const InputComponent& other);

#ifdef _CLIENT
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#elif _SERVER
    void Read(InputMemoryStream& inputStream, U32 dirtyState, bool isLocalPlayer) override;
#endif

    Vec2 m_acceleration;
    float m_angularAcceleration;
};
}

#endif
