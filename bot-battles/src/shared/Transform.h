#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class Transform {
public:
    Transform();
    Transform(const Vec3& position, F32 rotation);
    Transform(const Vec3& position, F32 rotation, F32 timestamp);

    F32 GetTimestamp() const;

public:
    Vec3 m_position;
    F32 m_rotation;

private:
    F32 m_timestamp;
};
}

#endif
