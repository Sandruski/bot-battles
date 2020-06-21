#ifndef __INPUT_MEMORY_STREAM_H__
#define __INPUT_MEMORY_STREAM_H__

#include "ByteSwap.h"
#include "NetUtils.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
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

    void ReadBits(U8& data, U32 bitCount);
    void ReadBits(void* data, U32 bitCount);

    template <typename T>
    void Read(T& data, U32 bitCount = sizeof(T) * 8);
    void Read(bool& data);
    template <typename T>
    void Read(std::vector<T>& vector);
    template <typename T, typename U>
    void Read(std::unordered_map<T, U>& unorderedMap);
    void Read(std::string& string);
    void Read(glm::vec2& vec2);
    void Read(glm::vec3& vec3);
    void Read(glm::vec4& vec4);
    void Read(glm::uvec2& uvec2);
    void Read(glm::uvec3& uvec3);
    void Read(glm::uvec4& uvec4);

    char* GetPtr() const;
    U32 GetByteLength() const;
    U32 GetByteCapacity() const;
    U32 GetRemainingBitCount() const;

    void SetCapacity(U32 byteCapacity);
    void SetHead(U32 bitCount);
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
    std::size_t size;
    Read(size);
    T element;
    while (size > 0) {
        Read(element);
        outVector.emplace_back(element);
        --size;
    }
}

//----------------------------------------------------------------------------------------------------
template <typename T, typename U>
inline void InputMemoryStream::Read(std::unordered_map<T, U>& outUnorderedMap)
{
    std::size_t size;
    Read(size);
    std::pair<T, U> pair;
    while (size > 0) {
        Read(pair.first);
        Read(pair.second);
        outUnorderedMap.insert(pair);
        --size;
    }
}
}

#endif
