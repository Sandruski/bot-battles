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

    void operator-=(const Vec2I& vec)
    {
        x -= vec.x;
        y -= vec.y;
    }

    void operator*=(I32 val)
    {
        x *= val;
        y *= val;
    }

    void operator/=(I32 val)
    {
        if (val != 0) {
            x /= val;
            y /= val;
        }
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
