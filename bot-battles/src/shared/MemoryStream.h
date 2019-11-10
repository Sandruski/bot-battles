#ifndef __MEMORY_STREAM_H__
#define __MEMORY_STREAM_H__

#include "ByteSwap.h"
#include "NetDefs.h"

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
// Bit stream
class OutputMemoryStream {
public:
    OutputMemoryStream();
    ~OutputMemoryStream();

    void WriteBits(U8 inData, U32 bitCount);
    void WriteBits(const void* inData, U32 bitCount);
    void WriteBytes(const void* inData, U32 byteCount);

    template <typename T>
    void Write(T inData, U32 bitCount = sizeof(T) * 8);
    void Write(bool inData);
    template <typename T>
    void Write(const std::vector<T>& inVector);
    void Write(const std::string& inString);
    void Write(const char* inString);
    void Write(const Vec2& inVec);
    void WritePosition(const Vec2& inVec);

    const char* GetPtr() const
    {
        return m_buffer;
    }

    U32 GetByteLength() const
    {
        return BITS_TO_BYTES(m_head + 7);
    }

private:
    void Realloc(U32 bitCapacity);

private:
    char* m_buffer;
    U32 m_head;
    U32 m_capacity;
};

//----------------------------------------------------------------------------------------------------
template <typename T>
inline void OutputMemoryStream::Write(T inData, U32 bitCount)
{
    static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Data is a non-primitive type");

    if (STREAM_ENDIANNESS == PLATFORM_ENDIANNESS()) {
        WriteBits(&inData, bitCount);
    } else {
        T swappedData = ByteSwap(inData);
        WriteBits(&swappedData, bitCount);
    }
}

//----------------------------------------------------------------------------------------------------
template <typename T>
inline void OutputMemoryStream::Write(const std::vector<T>& inVector)
{
    static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Data is a non-primitive type");

    std::size_t size = inVector.size();
    Write(size);

    for (const T& element : inVector) {
        Write(element);
    }
}

//----------------------------------------------------------------------------------------------------
// Bit stream
class InputMemoryStream {
public:
    InputMemoryStream();
    ~InputMemoryStream();

    void ReadBits(U8& outData, U32 bitCount);
    void ReadBits(void* outData, U32 bitCount);
    void ReadBytes(void* outData, U32 byteCount);

    template <typename T>
    void Read(T& outData, U32 bitCount = sizeof(T) * 8);
    void Read(bool& outData);
    template <typename T>
    void Read(std::vector<T>& outVector);
    void Read(std::string& outString);
    void Read(char* outString);
    void Read(Vec2& outVec);
    void ReadPosition(Vec2& outVec);

    char* GetPtr() const
    {
        return m_buffer;
    }

    U32 GetByteCapacity() const
    {
        return BITS_TO_BYTES(m_capacity);
    }

    U32 GetRemainingBitCount() const
    {
        return m_capacity - m_head;
    }

    void SetCapacity(U32 byteCapacity);
    void AdvanceHead(U32 bitCount);
    void ResetHead();

private:
    void Alloc(U32 bitCapacity);

private:
    char* m_buffer;
    U32 m_head;
    U32 m_capacity;
};

//----------------------------------------------------------------------------------------------------
template <typename T>
inline void InputMemoryStream::Read(T& outData, U32 bitCount)
{
    static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Data is a non-primitive type");

    if (STREAM_ENDIANNESS == PLATFORM_ENDIANNESS()) {
        ReadBits(&outData, bitCount);
    } else {
        T unswappedData;
        ReadBits(&unswappedData, bitCount);
        outData = ByteSwap(unswappedData);
    }
}

//----------------------------------------------------------------------------------------------------
template <typename T>
inline void InputMemoryStream::Read(std::vector<T>& outVector)
{
    static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Data is a non-primitive type");

    std::size_t size;
    Read(size);
    outVector.resize(size);

    for (const T& element : outVector) {
        Read(element);
    }
}
}

#endif
