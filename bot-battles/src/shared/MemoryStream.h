#ifndef __MEMORY_STREAM_H__
#define __MEMORY_STREAM_H__

#include "ByteSwap.h"
#include "NetUtils.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
// Bit stream
class OutputMemoryStream {
public:
    OutputMemoryStream();
    ~OutputMemoryStream();

    void WriteBits(U8 inData, U32 bitCount);
    void WriteBits(const void* inData, U32 bitCount);

    template <typename T>
    void Write(T inData, U32 bitCount = sizeof(T) * 8);
    void Write(bool inData);
    template <typename T>
    void Write(const std::vector<T>& inVector);
    void Write(const std::string& inString);
    void Write(const Vec2& inVec);
    void Write(const Vec3& inVec);
    void Write(const SDL_Color& inColor);
    void WritePosition(const Vec2& inVec);

    const char* GetPtr() const;
    U32 GetByteLength() const;

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
    InputMemoryStream(const InputMemoryStream& other);
    ~InputMemoryStream();

    InputMemoryStream& operator=(const InputMemoryStream& other)
    {
        m_head = other.m_head;

        std::free(m_buffer);
        Alloc(other.m_capacity);
        std::memcpy(m_buffer, other.m_buffer, BITS_TO_BYTES(m_capacity));

        return *this;
    }

    void ReadBits(U8& outData, U32 bitCount);
    void ReadBits(void* outData, U32 bitCount);

    template <typename T>
    void Read(T& outData, U32 bitCount = sizeof(T) * 8);
    void Read(bool& outData);
    template <typename T>
    void Read(std::vector<T>& outVector);
    void Read(std::string& outString);
    void Read(Vec2& outVec);
    void Read(Vec3& outVec);
    void Read(SDL_Color& outColor);
    void ReadPosition(Vec2& outVec);

    char* GetPtr() const;
    U32 GetByteCapacity() const;
    U32 GetRemainingBitCount() const;

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
