#include "Vec2.h"

namespace sand {

const Vec2 Vec2::zero = Vec2();
const Vec2 Vec2::one = Vec2(1.0f, 1.0f);
const Vec2 Vec2::positiveInfinity = Vec2(FLT_MAX, FLT_MAX);
const Vec2 Vec2::negativeInfinity = Vec2(-FLT_MAX, -FLT_MAX);

//----------------------------------------------------------------------------------------------------
Vec2::Vec2()
    : x(0.0f)
    , y(0.0f)
{
}

//----------------------------------------------------------------------------------------------------
Vec2::Vec2(F32 x, F32 y)
    : x(x)
    , y(y)
{
}
}
