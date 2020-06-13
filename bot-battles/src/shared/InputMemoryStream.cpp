#include "InputMemoryStream.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
InputMemoryStream::InputMemoryStream()
    : m_buffer(nullptr)
    , m_head(0)
    , m_capacity(0)
{
    Alloc(BYTES_TO_BITS(MAX_PACKET_SIZE));
}

//----------------------------------------------------------------------------------------------------
InputMemoryStream::InputMemoryStream(const InputMemoryStream& other)
    : m_buffer(nullptr)
    , m_head(other.m_head)
    , m_capacity(0)
{
    Alloc(other.m_capacity);
    std::memcpy(m_buffer, other.m_buffer, BITS_TO_BYTES(m_capacity));
}

//----------------------------------------------------------------------------------------------------
InputMemoryStream::~InputMemoryStream()
{
    std::free(m_buffer);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::ReadBits(void* data, U32 bitCount)
{
    U8* head = reinterpret_cast<U8*>(data);

    // Read the bytes
    U32 currentBitCount = bitCount;
    while (currentBitCount > 8) {
        ReadBits(*head, 8);
        ++head;

        currentBitCount -= 8;
    }

    // Read the remaining bits (if any)
    if (currentBitCount > 0) {
        ReadBits(*head, currentBitCount);
    }
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::ReadBits(U8& data, U32 bitCount)
{
    U32 nextHead = m_head + bitCount;

    U32 currentByteOffset = BITS_TO_BYTES(m_head);
    // 1 byte = 8 bits
    U32 currentBitOffset = m_head & 0x7; // bits shifted away in the previous step
    // 0x7 = 0b111

    U8 currentByteData = static_cast<U8>(m_buffer[currentByteOffset]) >> currentBitOffset; // written data of the current byte
    data = currentByteData;

    U32 currentByteBitsFree = 8 - currentBitOffset; // bits free in the current byte (some of them now contain in data)
    if (currentByteBitsFree < bitCount) {
        // We need another byte
        U8 nextByteData = static_cast<U8>(m_buffer[currentByteOffset + 1]) << currentByteBitsFree; // written data of the next byte
        data |= nextByteData;
    }

    U8 outByteDataMask = ~(0xFF << bitCount);
    // 0xFF << bitCount is 0xFF00000 if bitCount is 5. Since it occupies 1 byte, it is 0x11100000 (and not 0x1111111100000)
    // ~(0xFF << bitCount) is 0x00FFFFF if bitCount is 5. Since it occupies 1 byte, it is 0x00011111 (and not 0x0000000011111)
    data &= outByteDataMask;

    m_head = nextHead;
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(bool& data)
{
    ReadBits(&data, 1);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(std::string& string)
{
    std::size_t size;
    Read(size);
    string.resize(size);

    for (auto& element : string) {
        Read(element);
    }
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(glm::vec2& vec2)
{
    Read(vec2.x);
    Read(vec2.y);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(glm::vec3& vec3)
{
    Read(vec3.x);
    Read(vec3.y);
    Read(vec3.z);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(glm::vec4& vec4)
{
    Read(vec4.x);
    Read(vec4.y);
    Read(vec4.z);
    Read(vec4.w);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(glm::uvec2& uvec2)
{
    Read(uvec2.x);
    Read(uvec2.y);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(glm::uvec3& uvec3)
{
    Read(uvec3.x);
    Read(uvec3.y);
    Read(uvec3.z);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(glm::uvec4& uvec4)
{
    Read(uvec4.x);
    Read(uvec4.y);
    Read(uvec4.z);
    Read(uvec4.w);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::ReadPosition(glm::vec2& vec2) // TODO: this should be done properly and without hardcoded values
{
    U32 x = 0;
    Read(x);
    U32 y = 0;
    Read(y);
    vec2.x = FIXED_TO_FLOAT(x, -2000.0f, 0.1f);
    vec2.y = FIXED_TO_FLOAT(y, -2000.0f, 0.1f);
}

//----------------------------------------------------------------------------------------------------
char* InputMemoryStream::GetPtr() const
{
    return m_buffer;
}

//----------------------------------------------------------------------------------------------------
U32 InputMemoryStream::GetByteLength() const
{
    return BITS_TO_BYTES(m_head + 7);
}

//----------------------------------------------------------------------------------------------------
U32 InputMemoryStream::GetByteCapacity() const
{
    return BITS_TO_BYTES(m_capacity);
}

//----------------------------------------------------------------------------------------------------
U32 InputMemoryStream::GetRemainingBitCount() const
{
    return m_capacity - m_head;
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::SetCapacity(U32 byteCapacity)
{
    m_capacity = BYTES_TO_BITS(byteCapacity);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::SetHead(U32 bitCount)
{
    assert(bitCount <= m_capacity);

    m_head = bitCount;
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::AdvanceHead(U32 bitCount)
{
    U32 nextHead = m_head + bitCount;
    assert(nextHead <= m_capacity);

    m_head = nextHead;
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::ResetHead()
{
    m_head = 0;
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Alloc(U32 bitCapacity)
{
    assert(m_buffer == nullptr);

    U32 newByteCapacity = BITS_TO_BYTES(bitCapacity);
    m_buffer = static_cast<char*>(std::malloc(newByteCapacity));
    assert(m_buffer != nullptr);
    std::memset(m_buffer, 0, newByteCapacity);

    m_capacity = bitCapacity;
}
}
