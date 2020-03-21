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

    void WriteBits(U8 data, U32 bitCount);
    void WriteBits(const void* data, U32 bitCount);

    template <typename T>
    void Write(T data, U32 bitCount = sizeof(T) * 8);
    void Write(bool data);
    template <typename T>
    void Write(const std::vector<T>& vector);
    template <typename T, typename U>
    void Write(const std::unordered_map<T, U>& unorderedMap);
    void Write(const std::string& string);
    void Write(const glm::vec2& vec2);
    void Write(const glm::vec3& vec3);
    void Write(const glm::vec4& vec4);
    void Write(const glm::uvec2& uvec2);
    void Write(const glm::uvec3& uvec3);
    void Write(const glm::uvec4& uvec4);
    void WritePosition(const glm::vec2& vec2);

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
    std::size_t size = inVector.size();
    Write(size);

    for (const T& element : inVector) {
        Write(element);
    }
}

//----------------------------------------------------------------------------------------------------
template <typename T, typename U>
inline void OutputMemoryStream::Write(const std::unordered_map<T, U>& inUnorderedMap)
{
    std::size_t size = inUnorderedMap.size();
    Write(size);

    for (const auto& pair : inUnorderedMap) {
        Write(pair.first);
        Write(pair.second);
    }
}
}

#endif
