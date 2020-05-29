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
    void Read(InputMemoryStream& inputStream, U64 dirtyState, U32 frame, ReplicationActionType replicationActionType, Entity entity) override;
#elif defined(_SERVER)
    U64 Write(OutputMemoryStream& outputStream, U64 dirtyState) const override;
#endif

    void SetAsCircle(glm::vec2 position, F32 rotation, F32 radius, bool isSensor, const Entity& entity);
    void SetAsBox(glm::vec2 position, F32 rotation, glm::vec2 halfSize, bool isSensor, const Entity& entity);

    void UpdateBodyType() const;
    void UpdateGroupIndex() const;
    void UpdateBullet() const;

    glm::vec2 GetLinearVelocity() const;

    // Bot
    F32 GetLinearVelocityX() const;
    F32 GetLinearVelocityY() const;
    F32 GetAngularVelocity() const;

    // Local
    b2Body* m_body;

    // Networked
    BodyType m_bodyType;
    I16 m_groupIndex;
    bool m_isBullet;
};
}

#endif
