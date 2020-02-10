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

    Vec2 operator+(const Vec2& vec) const
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

    Vec2 operator-(const Vec2& vec) const
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

    Vec2 operator*(F32 val) const
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

    Vec2 operator/(F32 val) const
    {
        Vec2 newVec(x, y);
        if (val != 0.0f) {
            newVec.x /= val;
            newVec.y /= val;
        }
        return newVec;
    }

    bool operator==(const Vec2& vec) const
    {
        return (x == vec.x && y == vec.y);
    }

    bool operator!=(const Vec2& vec) const
    {
        return !(x == vec.x && y == vec.y);
    }

    bool operator<(const Vec2& vec) const
    {
        return (x < vec.x && y < vec.y);
    }

    bool operator<=(const Vec2& vec) const
    {
        return (x <= vec.x && y <= vec.y);
    }

    bool operator>(const Vec2& vec) const
    {
        return (x > vec.x && y > vec.y);
    }

    bool operator>=(const Vec2& vec) const
    {
        return (x >= vec.x && y >= vec.y);
    }

    inline Vec2 GetNormalized()
    {
        return Vec2::Normalize(*this);
    }

    static Vec2 GetNormalized(Vec2 vec)
    {
        return Vec2::Normalize(vec);
    }

    inline void Normalize()
    {
        F32 mag = GetMagnitude();
        if (mag > EPSILON)
        {
            *this /= mag;
        }
        else
        {
            *this = Vec2::zero;
        }
    }

    static Vec2 Normalize(Vec2 vec)
    {
        F32 mag = GetMagnitude(vec);
        if (mag > EPSILON)
        {
            return vec / mag;
        }
        else
        {
            return Vec2::zero;
        }
    }

    inline F32 GetMagnitude()
    { 
        return sqrt(x * x + y * y);
    }

    static F32 GetMagnitude(Vec2 vec)
    {
        return sqrt(vec.x * vec.x + vec.y * vec.y);
    }

public:
    static const Vec2 zero;
    static const Vec2 one;
    static const Vec2 positiveInfinity;
    static const Vec2 negativeInfinity;

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
