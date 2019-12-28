#ifndef __VEC_2_H__
#define __VEC_2_H__

#include "Memory.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class Vec2 {
public:
    Vec2();
    Vec2(F32 x, F32 y);

    void operator+=(const Vec2& vec)
    {
        x += vec.x;
        y += vec.y;
    }

    Vec2 operator+(const Vec2& vec)
    {
        Vec2 newVec(x, y);
        newVec.x += vec.x;
        newVec.y += vec.y;
        return newVec;
    }

    void operator-=(const Vec2& vec)
    {
        x -= vec.x;
        y -= vec.y;
    }

    Vec2 operator-(const Vec2& vec)
    {
        Vec2 newVec(x, y);
        newVec.x -= vec.x;
        newVec.y -= vec.y;
        return newVec;
    }

    void operator*=(F32 val)
    {
        x *= val;
        y *= val;
    }

    Vec2 operator*(F32 val)
    {
        Vec2 newVec(x, y);
        newVec.x *= val;
        newVec.y *= val;
        return newVec;
    }

    void operator/=(F32 val)
    {
        if (val != 0.0f) {
            x /= val;
            y /= val;
        }
    }

    Vec2 operator/(F32 val)
    {
        Vec2 newVec(x, y);
        if (val != 0.0f) {
            newVec.x /= val;
            newVec.y /= val;
        }
        return newVec;
    }

public:
    static const Vec2 zero;
    static const Vec2 one;

public:
#pragma warning(disable : 4201)
    union {
        struct
        {
            F32 x;
            F32 y;
        };

        F32 coord[2];
    };
#pragma warning(default : 4201)
};
}

#endif
