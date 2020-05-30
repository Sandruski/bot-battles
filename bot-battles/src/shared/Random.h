#ifndef __RANDOM_H__
#define __RANDOM_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
inline I32 RandomInt(I32 min, I32 max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<I32> dis(min, max);
    return dis(gen);
}

//----------------------------------------------------------------------------------------------------
inline F32 RandomFloat(F32 min, F32 max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<F32> dis(min, max);
    return dis(gen);
}
}
#endif
