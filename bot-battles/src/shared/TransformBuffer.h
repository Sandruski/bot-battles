#ifndef __TRANSFORM_BUFFER_H__
#define __TRANSFORM_BUFFER_H__

#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class TransformBuffer {
public:
    TransformBuffer();

    void Add(const TransformComponent& transformComponent);
    void Remove(U32 frame);
    void Clear();

    U32 GetIndex(U32 frame) const;
    TransformComponent& GetTransform(U32 index);
    TransformComponent& GetLastTransform();

    bool IsFull() const;
    U32 GetCount() const;

public:
    U32 m_front;
    U32 m_back;

private:
    std::array<TransformComponent, MAX_TRANSFORMS> m_transforms;
};
}

#endif
