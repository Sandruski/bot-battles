#ifndef __OUTPUT_MEMORY_STREAM_H__
#define __OUTPUT_MEMORY_STREAM_H__

#include "ByteSwap.h"
#include "NetUtils.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
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
}

#endif
