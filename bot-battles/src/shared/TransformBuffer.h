#ifndef __TRANSFORM_BUFFER_H__
#define __TRANSFORM_BUFFER_H__

#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class TransformBuffer {
public:
    TransformBuffer();

    void Add(const TransformComponent& transformComponent);
    void RemoveUntilFrame(U32 frame);
    void Clear();
    TransformComponent& GetTransformComponentAtFrame(U32 frame);

    U32 GetCount() const;
    TransformComponent& Get(U32 index);

private:
    std::deque<std::pair<U32, TransformComponent>> m_transforms;
};
}

#endif
