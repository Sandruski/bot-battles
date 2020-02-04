#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class Transform {
public:
    Transform();
    Transform(const Vec3& position, F32 rotation);
    Transform(const Vec3& position, F32 rotation, F32 timestamp, U32 frame);

    F32 GetTimestamp() const;
    U32 GetFrame() const;

public:
    Vec3 m_position;
    F32 m_rotation;

private:
    F32 m_timestamp;
    U32 m_frame;
};
}

#endif
