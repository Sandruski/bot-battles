#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "TransformComponent.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class Transform {
public:
    Transform();
    Transform(const TransformComponent& transformComponent, U32 frame);

    const TransformComponent& GetTransformComponent() const;
    U32 GetFrame() const;

private:
    TransformComponent m_transformComponent;
    U32 m_frame;
};
}

#endif
