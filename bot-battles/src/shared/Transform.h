#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class Transform {
public:
    Transform();
    Transform(const glm::vec2& position, F32 rotation, U32 frame);

    void UpdateTransform(const glm::vec2& acceleration, F32 angularAcceleration, F32 dt);
    void UpdatePosition(const glm::vec2& acceleration, F32 dt);
    void UpdateRotation(F32 angularAcceleration, F32 dt);

    U32 GetFrame() const;

public:
    glm::vec2 m_position;
    F32 m_rotation;

private:
    U32 m_frame;
};
}

#endif
