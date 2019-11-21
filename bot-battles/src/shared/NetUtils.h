#ifndef __NET_UTILS_H__
#define __NET_UTILS_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
inline U32 BITS_TO_BYTES(U32 bits)
{
    return bits >> 3;
    // value >> 1 is dividing by 2, value >> 2 is dividing by 4, value >> 3 is dividing by 8, ... value >> n is dividing by 2^n
}

//----------------------------------------------------------------------------------------------------
inline U32 BYTES_TO_BITS(U32 bytes)
{
    return bytes << 3;
    // value << 1 is multiplying by 2, value << 2 is multiplying by 4, value << 3 is multiplying by 8, ... value << n is multiplying by 2^n
}

//----------------------------------------------------------------------------------------------------
// Fixed point
inline U32 FLOAT_TO_FIXED(F32 number, F32 min, F32 precision)
{
    return static_cast<U32>((number - min) / precision);
}

//----------------------------------------------------------------------------------------------------
// Fixed point
inline F32 FIXED_TO_FLOAT(U32 number, F32 min, F32 precision)
{
    return static_cast<F32>(number) * precision + min;
}

//----------------------------------------------------------------------------------------------------
template <U32 value, U32 bits>
struct GetRequiredBitsHelper {
    enum { value = GetRequiredBitsHelper<(value >> 1), bits + 1>::value };
};

//----------------------------------------------------------------------------------------------------
template <U32 bits>
struct GetRequiredBitsHelper<0, bits> {
    enum { value = bits };
};

//----------------------------------------------------------------------------------------------------
template <U32 value>
struct GetRequiredBits {
    enum { value = GetRequiredBitsHelper<value, 0>::value };
};
}

#endif
