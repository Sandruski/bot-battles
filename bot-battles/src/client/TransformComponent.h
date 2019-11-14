#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "MemoryStream.h"
#include "NetComponent.h"
#include "ReplicationCommand.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct TransformComponent : public ReadNetComponent {

    enum class MemberType : U16 {
        POSITION = 1 << 0,
        ROTATION = 1 << 1,

        COUNT,
        INVALID,

        ALL = POSITION | ROTATION
    };

    static ComponentType GetType() { return ComponentType::TRANSFORM; }
    static TransformComponent* Instantiate() { return new TransformComponent(); }

    TransformComponent()
        : m_position()
        , m_rotation(0.0f)
    {
    }
    ~TransformComponent() { }

    void Read(InputMemoryStream& inputStream) override
    {
        U16 memberFlags = 0;
        inputStream.Read(memberFlags, GetRequiredBits<static_cast<U16>(TransformComponent::MemberType::COUNT)>::value);
        if (memberFlags & static_cast<U16>(MemberType::POSITION)) {
            inputStream.Read(m_position);
        }
        if (memberFlags & static_cast<U16>(MemberType::ROTATION)) {
            inputStream.Read(m_rotation);
        }
    }

    Vec2 m_position;
    float m_rotation;
};
}

#endif
