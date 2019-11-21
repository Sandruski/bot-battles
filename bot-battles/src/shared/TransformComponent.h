#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "ComponentMemberTypes.h"
#include "MemoryStream.h"
#include "NetComponent.h"
#include "ReplicationCommand.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
struct TransformComponent : public NetComponent {

    static ComponentType GetType() { return ComponentType::TRANSFORM; }
    static TransformComponent* Instantiate() { return new TransformComponent(); }

    TransformComponent()
        : m_position()
        , m_rotation(0.0f)
    {
    }
    ~TransformComponent() { }

    U32 Write(OutputMemoryStream& outputStream, U32 dirtyState) const override
    {
        U32 writtenState = 0;

        if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION)) {
            outputStream.WritePosition(m_position);
            writtenState |= static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION);
        }
        if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION)) {
            outputStream.Write(m_rotation);
            writtenState |= static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION);
        }

        return writtenState;
    }

    void Read(InputMemoryStream& inputStream, U32 dirtyState) override
    {
        if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_POSITION)) {
            inputStream.ReadPosition(m_position);
        }
        if (dirtyState & static_cast<U32>(ComponentMemberType::TRANSFORM_ROTATION)) {
            inputStream.Read(m_rotation);
        }
    }

    Vec2 m_position;
    float m_rotation;
};
}

#endif
