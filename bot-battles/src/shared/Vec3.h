#ifndef __VEC_3_H__
#define __VEC_3_H__

#include "Memory.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class Vec3 {
public:
    Vec3();
    Vec3(F32 x, F32 y, F32 z);

    void operator+=(const Vec3& vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;
    }

    Vec3 operator+(const Vec3& vec) const
    {
        Vec3 newVec(x, y, z);
        newVec.x += vec.x;
        newVec.y += vec.y;
        newVec.z += vec.z;
        return newVec;
    }

    void operator-=(const Vec3& vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
    }

    Vec3 operator-(const Vec3& vec) const
    {
        Vec3 newVec(x, y, z);
        newVec.x -= vec.x;
        newVec.y -= vec.y;
        newVec.z -= vec.z;
        return newVec;
    }

    void operator*=(F32 val)
    {
        x *= val;
        y *= val;
        z *= val;
    }

    Vec3 operator*(F32 val) const
    {
        Vec3 newVec(x, y, z);
        newVec.x *= val;
        newVec.y *= val;
        newVec.z *= val;
        return newVec;
    }

    void operator/=(F32 val)
    {
        if (val != 0.0f) {
            x /= val;
            y /= val;
            z /= val;
        }
    }

    Vec3 operator/(F32 val) const
    {
        Vec3 newVec(x, y, z);
        if (val != 0.0f) {
            newVec.x /= val;
            newVec.y /= val;
            newVec.z /= val;
        }
        return newVec;
    }

    bool operator==(const Vec3& vec) const
    {
        return (x == vec.x && y == vec.y && z == vec.z);
    }

    bool operator!=(const Vec3& vec) const
    {
        return !(x == vec.x && y == vec.y && z == vec.z);
    }

public:
    static const Vec3 zero;
    static const Vec3 one;

public:
#pragma warning(disable : 4201)
    union {
        struct
        {
            F32 x;
            F32 y;
            F32 z;
        };

        F32 coord[3];
    };
#pragma warning(default : 4201)
};
}

#endif
