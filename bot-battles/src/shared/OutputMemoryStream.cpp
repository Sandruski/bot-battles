#include "OutputMemoryStream.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
OutputMemoryStream::OutputMemoryStream()
    : m_buffer(nullptr)
    , m_head(0)
    , m_capacity(0)
{
    Realloc(BYTES_TO_BITS(MAX_PACKET_SIZE));
}

//----------------------------------------------------------------------------------------------------
OutputMemoryStream::~OutputMemoryStream()
{
    std::free(m_buffer);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::WriteBits(const void* data, U32 bitCount)
{
    const char* head = static_cast<const char*>(data);

    // Break data into bytes and write the bytes
    U32 currentBitCount = bitCount;
    while (currentBitCount > 8) {
        WriteBits(*head, 8);
        ++head;

        currentBitCount -= 8;
    }

    // Write the remaining bits (if any)
    if (currentBitCount > 0) {
        WriteBits(*head, currentBitCount);
    }
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::WriteBits(U8 data, U32 bitCount)
{
    U32 nextHead = m_head + bitCount;
    assert(nextHead <= m_capacity);
    /*
    if (nextHead > m_capacity) {
        Realloc(std::max(m_capacity * 2, nextHead));
    }
    */

    U32 currentByteOffset = BITS_TO_BYTES(m_head);
    // 1 byte = 8 bits
    U32 currentBitOffset = m_head & 0x7; // bits shifted away in the previous step
    // 0x7 = 0b111

    U8 currentByteDataMask = ~(0xFF << currentBitOffset);
    // 0xFF << bitOffset is 0xFF00000 if bitOffset is 5. Since it occupies 1 byte, it is 0x11100000 (and not 0x1111111100000)
    // ~(0xFF << bitOffset) is 0x00FFFFF if bitOffset is 5. Since it occupies 1 byte, it is 0x00011111 (and not 0x0000000011111)
    U8 currentByteData = m_buffer[currentByteOffset] & currentByteDataMask; // written data of the current byte
    U8 currentByteInData = data << currentBitOffset; // to write data for the current byte
    m_buffer[currentByteOffset] = currentByteData | currentByteInData;

    U32 currentByteBitsFree = 8 - currentBitOffset; // bits free in the current byte (some of them now contain in data)
    if (currentByteBitsFree < bitCount) {
        // We need another byte
        U8 nextByteInData = data >> currentByteBitsFree; // to write data for the next byte
        m_buffer[currentByteOffset + 1] = nextByteInData;
    }

    m_head = nextHead;
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(bool data)
{
    WriteBits(&data, 1);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const std::string& string)
{
    std::size_t size = string.size();
    Write(size);

    for (const auto& element : string) {
        Write(element);
    }
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const glm::vec2& vec2)
{
    Write(vec2.x);
    Write(vec2.y);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const glm::vec3& vec3)
{
    Write(vec3.x);
    Write(vec3.y);
    Write(vec3.z);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const glm::vec4& vec4)
{
    Write(vec4.x);
    Write(vec4.y);
    Write(vec4.z);
    Write(vec4.w);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const glm::uvec2& uvec2)
{
    Write(uvec2.x);
    Write(uvec2.y);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const glm::uvec3& uvec3)
{
    Write(uvec3.x);
    Write(uvec3.y);
    Write(uvec3.z);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const glm::uvec4& uvec4)
{
    Write(uvec4.x);
    Write(uvec4.y);
    Write(uvec4.z);
    Write(uvec4.w);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::WritePosition(const glm::vec2& vec2) // TODO: this should be done properly and without hard-coded values
{
    U32 x = FLOAT_TO_FIXED(vec2.x, -2000.0f, 0.1f);
    U32 y = FLOAT_TO_FIXED(vec2.y, -2000.0f, 0.1f);
    Write(x);
    Write(y);
}

//----------------------------------------------------------------------------------------------------
const char* OutputMemoryStream::GetPtr() const
{
    return m_buffer;
}

//----------------------------------------------------------------------------------------------------
U32 OutputMemoryStream::GetByteLength() const
{
    return BITS_TO_BYTES(m_head + 7);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Realloc(U32 bitCapacity)
{
    U32 newByteCapacity = BITS_TO_BYTES(bitCapacity);

    if (m_buffer == nullptr) {
        m_buffer = static_cast<char*>(std::malloc(newByteCapacity));
        assert(m_buffer != nullptr);
        std::memset(m_buffer, 0, newByteCapacity);
    } else {
        m_buffer = static_cast<char*>(std::realloc(m_buffer, newByteCapacity));
        assert(m_buffer != nullptr);
        U32 oldByteCapacity = BITS_TO_BYTES(m_capacity);
        char* newBuffer = m_buffer + oldByteCapacity;
        U32 newBufferCapacity = newByteCapacity - oldByteCapacity;
        std::memset(newBuffer, 0, newBufferCapacity);
    }

    m_capacity = bitCapacity;
}
}
