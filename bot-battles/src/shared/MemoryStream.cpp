#include "MemoryStream.h"

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
void OutputMemoryStream::WriteBits(const void* inData, U32 bitCount)
{
    const char* head = static_cast<const char*>(inData);

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
void OutputMemoryStream::WriteBits(U8 inData, U32 bitCount)
{
    U32 nextHead = m_head + bitCount;
    if (nextHead > m_capacity) {
        Realloc(std::max(m_capacity * 2, nextHead));
    }

    U32 currentByteOffset = BITS_TO_BYTES(m_head);
    // 1 byte = 8 bits
    U32 currentBitOffset = m_head & 0x7; // bits shifted away in the previous step
    // 0x7 = 0b111

    U8 currentByteDataMask = ~(0xFF << currentBitOffset);
    // 0xFF << bitOffset is 0xFF00000 if bitOffset is 5. Since it occupies 1 byte, it is 0x11100000 (and not 0x1111111100000)
    // ~(0xFF << bitOffset) is 0x00FFFFF if bitOffset is 5. Since it occupies 1 byte, it is 0x00011111 (and not 0x0000000011111)
    U8 currentByteData = m_buffer[currentByteOffset] & currentByteDataMask; // written data of the current byte
    U8 currentByteInData = inData << currentBitOffset; // to write data for the current byte
    m_buffer[currentByteOffset] = currentByteData | currentByteInData;

    U32 currentByteBitsFree = 8 - currentBitOffset; // bits free in the current byte (some of them now contain in data)
    if (currentByteBitsFree < bitCount) {
        // We need another byte
        U8 nextByteInData = inData >> currentByteBitsFree; // to write data for the next byte
        m_buffer[currentByteOffset + 1] = nextByteInData;
    }

    m_head = nextHead;
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(bool inData)
{
    WriteBits(&inData, 1);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const std::string& inString)
{
    std::size_t size = inString.size();
    Write(size);

    for (const auto& element : inString) {
        Write(element);
    }
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const Vec2& inVec)
{
    Write(inVec.x);
    Write(inVec.y);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const Vec3& inVec)
{
    Write(inVec.x);
    Write(inVec.y);
    Write(inVec.z);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const SDL_Color& inColor)
{
    Write(inColor.r);
    Write(inColor.g);
    Write(inColor.b);
    Write(inColor.a);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::WritePosition(const Vec2& inVec) // TODO: this should be done properly and without hard-coded values
{
    U32 x = FLOAT_TO_FIXED(inVec.x, -2000.0f, 0.1f);
    U32 y = FLOAT_TO_FIXED(inVec.y, -2000.0f, 0.1f);
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
        std::memset(m_buffer + oldByteCapacity, 0, newByteCapacity);
    }

    m_capacity = bitCapacity;
}

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
void InputMemoryStream::ReadBits(void* outData, U32 bitCount)
{
    U8* head = reinterpret_cast<U8*>(outData);

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
void InputMemoryStream::ReadBits(U8& outData, U32 bitCount)
{
    U32 nextHead = m_head + bitCount;
    assert(nextHead <= m_capacity);

    U32 currentByteOffset = BITS_TO_BYTES(m_head);
    // 1 byte = 8 bits
    U32 currentBitOffset = m_head & 0x7; // bits shifted away in the previous step
    // 0x7 = 0b111

    U8 currentByteData = static_cast<U8>(m_buffer[currentByteOffset]) >> currentBitOffset; // written data of the current byte
    outData = currentByteData;

    U32 currentByteBitsFree = 8 - currentBitOffset; // bits free in the current byte (some of them now contain in data)
    if (currentByteBitsFree < bitCount) {
        // We need another byte
        U8 nextByteData = static_cast<U8>(m_buffer[currentByteOffset + 1]) << currentByteBitsFree; // written data of the next byte
        outData |= nextByteData;
    }

    U8 outByteDataMask = ~(0xFF << bitCount);
    // 0xFF << bitCount is 0xFF00000 if bitCount is 5. Since it occupies 1 byte, it is 0x11100000 (and not 0x1111111100000)
    // ~(0xFF << bitCount) is 0x00FFFFF if bitCount is 5. Since it occupies 1 byte, it is 0x00011111 (and not 0x0000000011111)
    outData &= outByteDataMask;

    m_head = nextHead;
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(bool& outData)
{
    ReadBits(&outData, 1);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(std::string& outString)
{
    std::size_t size;
    Read(size);
    outString.resize(size);

    for (auto& element : outString) {
        Read(element);
    }
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(Vec2& outVec)
{
    Read(outVec.x);
    Read(outVec.y);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(Vec3& outVec)
{
    Read(outVec.x);
    Read(outVec.y);
    Read(outVec.z);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(SDL_Color& outColor)
{
    Read(outColor.r);
    Read(outColor.g);
    Read(outColor.b);
    Read(outColor.a);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::ReadPosition(Vec2& outVec) // TODO: this should be done properly and without hard-coded values
{
    U32 x = 0;
    Read(x);
    U32 y = 0;
    Read(y);
    outVec.x = FIXED_TO_FLOAT(x, -2000.0f, 0.1f);
    outVec.y = FIXED_TO_FLOAT(y, -2000.0f, 0.1f);
}

//----------------------------------------------------------------------------------------------------
char* InputMemoryStream::GetPtr() const
{
    return m_buffer;
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
