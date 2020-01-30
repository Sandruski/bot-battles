#ifndef __INTERPOLATION_H__
#define __INTERPOLATION_H__

namespace sand {

template <class T>
inline T Lerp(const T& from, const T& to, F32 t)
{
    return T(from + (to - from) * t);
}
}

#endif
