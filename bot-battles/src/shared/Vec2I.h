#ifndef __VEC_2_I_H__
#define __VEC_2_I_H__

#include "Memory.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class Vec2I {
public:
    Vec2I();
    Vec2I(I32 x, I32 y);
    ~Vec2I();

    void operator+=(const Vec2I& vec)
    {
        x += vec.x;
        y += vec.y;
    }

    Vec2I operator+(const Vec2I& vec)
    {
        Vec2I newVec(x, y);
        newVec.x += vec.x;
        newVec.y += vec.y;
        return newVec;
    }

    void operator-=(const Vec2I& vec)
    {
        x -= vec.x;
        y -= vec.y;
    }

    Vec2I operator-(const Vec2I& vec)
    {
        Vec2I newVec(x, y);
        newVec.x -= vec.x;
        newVec.y -= vec.y;
        return newVec;
    }

    void operator*=(I32 val)
    {
        x *= val;
        y *= val;
    }

    Vec2I operator*(I32 val)
    {
        Vec2I newVec(x, y);
        newVec.x *= val;
        newVec.y *= val;
        return newVec;
    }

    void operator/=(I32 val)
    {
        if (val != 0) {
            x /= val;
            y /= val;
        }
    }

    Vec2I operator/(I32 val)
    {
        Vec2I newVec(x, y);
        if (val != 0) {
            newVec.x /= val;
            newVec.y /= val;
        }
        return newVec;
    }

public:
    static const Vec2I zero;
    static const Vec2I one;

public:
#pragma warning(disable : 4201)
    union {
        struct
        {
            I32 x;
            I32 y;
        };

        I32 coord[2];
    };
#pragma warning(default : 4201)
};
}

#endif
