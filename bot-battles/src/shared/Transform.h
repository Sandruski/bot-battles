#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class Transform {
public:
    Transform();
    Transform(const glm::vec3& position, F32 rotation, U32 frame);

    U32 GetFrame() const;

public:
    glm::vec3 m_position;
    F32 m_rotation;

private:
    U32 m_frame;
};
}

#endif
