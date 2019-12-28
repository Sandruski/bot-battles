#include "Vec3.h"

namespace sand {

const Vec3 Vec3::zero = Vec3();
const Vec3 Vec3::one = Vec3(1.0f, 1.0f, 1.0f);

//----------------------------------------------------------------------------------------------------
Vec3::Vec3()
    : x(0.0f)
    , y(0.0f)
    , z(0.0f)
{
}

//----------------------------------------------------------------------------------------------------
Vec3::Vec3(F32 x, F32 y, F32 z)
    : x(x)
    , y(y)
    , z(z)
{
}
}
