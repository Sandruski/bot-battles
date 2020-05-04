#ifndef __RIGIDBODY_COMPONENT_H__
#define __RIGIDBODY_COMPONENT_H__

#include "Component.h"
#ifdef _CLIENT
#include "NetworkableReadObject.h"
#elif defined(_SERVER)
#include "NetworkableWriteObject.h"
#endif

namespace sand {

// Entity Component
struct RigidbodyComponent : public Component
#ifdef _CLIENT
    ,
                            public NetworkableReadObject
#elif defined(_SERVER)
    ,
                            public NetworkableWriteObject
#endif
{
    static ComponentType GetType() { return ComponentType::RIGIDBODY; }

    enum class BodyType {

        NONE,
        STATIC,
        DYNAMIC,
        KINEMATIC
    };

    RigidbodyComponent();
    ~RigidbodyComponent() override;

#ifdef _CLIENT
    void Read(InputMemoryStream& inputStream, U32 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override;
#endif

    void SetAsCircle(glm::vec2 position, F32 rotation, F32 radius, const Entity& entity);
    void SetAsBox(glm::vec2 position, F32 rotation, glm::vec2 halfSize, const Entity& entity);
    void SetAsBullet(bool isBullet);

    void UpdateBodyType() const;
    void UpdateGroupIndex() const;

    // Bot
    glm::vec2 GetLinearVelocity() const;
    F32 GetAngularVelocity() const;

    // Local
    b2Body* m_body;

    // Networked
    BodyType m_bodyType;
    I16 m_groupIndex;
};
}

#endif
