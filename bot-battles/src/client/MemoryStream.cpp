#include "MemoryStream.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
OutputMemoryStream::OutputMemoryStream()
    : m_buffer(nullptr)
    , m_head(0)
    , m_capacity(0)
{
	U32 bitCapacity = BYTES_TO_BITS(1500);
	Realloc(bitCapacity);
}

//----------------------------------------------------------------------------------------------------
OutputMemoryStream::~OutputMemoryStream()
{
	std::free(m_buffer);
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

	WriteBytes(inString.c_str(), size * sizeof(char));
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const char* inString)
{
	std::size_t size = std::strlen(inString);
	Write(size);

	WriteBytes(inString, size * sizeof(char));
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(Entity inEntity)
{
	//NetworkID networkID = mLinkingContext->GetNetworkID(entity);
	//Write(networkID);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const Vec2& inVec)
{
	Write(inVec.x);
	Write(inVec.y);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::WriteBits(const void* inData, U32 bitCount)
{
	const char* head = static_cast<const char*>(inData);
	
	// Break data into bytes and write the bytes
	U32 currentBitCount = bitCount;
	while (currentBitCount > 8)
	{
		WriteBits(*head, 8);
		++head;

		currentBitCount -= 8;
	}

	// Write the remaining bits (if any)
	if (currentBitCount > 0)
	{
		WriteBits(*head, currentBitCount);
	}
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::WriteBits(U8 inData, U32 bitCount)
{
	U32 nextHead = m_head + bitCount;
	if (nextHead > m_capacity)
	{
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
	if (currentByteBitsFree < bitCount)
	{
		// Overflow!!!
		U8 nextByteInData = inData >> currentByteBitsFree; // to write data for the next byte
		m_buffer[currentByteOffset + 1] = nextByteInData;
	}

	m_head = nextHead;
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::WriteBytes(const void* inData, U32 byteCount)
{
	U32 bitCount = BYTES_TO_BITS(byteCount);
	WriteBits(inData, bitCount);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Realloc(U32 bitCapacity)
{
	U32 newByteCapacity = BITS_TO_BYTES(bitCapacity);

	if (m_buffer == nullptr)
	{
		m_buffer = static_cast<char*>(std::malloc(newByteCapacity));
		assert(m_buffer != nullptr);
		std::memset(m_buffer, 0, newByteCapacity);
	}
	else
	{
		m_buffer = static_cast<char*>(std::realloc(m_buffer, newByteCapacity));
		assert(m_buffer != nullptr);
		U32 oldByteCapacity = BITS_TO_BYTES(m_capacity);
		std::memset(m_buffer + oldByteCapacity, 0, newByteCapacity);
	}

	m_capacity = bitCapacity;
}


//----------------------------------------------------------------------------------------------------
InputMemoryStream::InputMemoryStream(U32 byteCount)
	: m_buffer(nullptr),
	m_head(0),
	m_capacity(byteCount)
{
}

//----------------------------------------------------------------------------------------------------
InputMemoryStream::~InputMemoryStream()
{
	std::free(m_buffer);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(std::string& outString)
{
	std::size_t size;
	Read(size);
	outString.resize(size);

	Read(&outString, size * sizeof(char));
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(char* outString)
{
	std::size_t size;
	Read(size);

	Read(&outString, size * sizeof(char));
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(Entity& entity)
{
	//NetworkID networkID;
	//Read(networkID);
	//entity = mLinkingContext->GetEntity(networkID);
}

//----------------------------------------------------------------------------------------------------
void InputMemoryStream::Read(void* outData, U32 byteCount)
{
	U32 newHead = m_head + byteCount;
	assert(newHead <= m_capacity);

	std::memcpy(outData, m_buffer + m_head, byteCount);

	m_head = newHead;
}
}
