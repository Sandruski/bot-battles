#ifndef __TRANSFORM_BUFFER_H__
#define __TRANSFORM_BUFFER_H__

#include "Transform.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class TransformBuffer {
public:
    TransformBuffer();

    const Transform& Add(const TransformComponent& transformComponent, U32 frame);
    const Transform& Add(const Transform& transform);
    void Remove(U32 frame);
    void Clear();

    const Transform& Get(U32 index) const;
    const Transform& GetLast() const;
    bool HasTransforms() const;
    U32 GetCount() const;

private:
    std::deque<Transform> m_transforms;
};
}

#endif
