#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif _SERVER
#include "NetworkableWriteObject.h"
#endif

namespace sand {

struct InputComponent;

//----------------------------------------------------------------------------------------------------
// Entity Component
struct TransformComponent : public Component,
#ifdef _CLIENT
                            public NetworkableReadObject
#elif _SERVER
                            public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::TRANSFORM; }

    TransformComponent();

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, bool isLocalPlayer) override;
    void ClientPredictionForLocalPlayer();
    void ClientPredictionForRemotePlayer();
#elif _SERVER
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    void Move(const InputComponent& inputComponent, F32 dt);

    Vec3 m_position;
    F32 m_rotation;
};
}

#endif
