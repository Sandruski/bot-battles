#include "MemoryStream.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
OutputMemoryStream::OutputMemoryStream()
    : m_buffer(nullptr)
    , m_head(0)
    , m_capacity(0)
{
}

//----------------------------------------------------------------------------------------------------
OutputMemoryStream::~OutputMemoryStream()
{
	std::free(m_buffer);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const std::string& inString)
{
	std::size_t size = inString.size();
	Write(size);

	Write(inString.data(), size * sizeof(char));
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const char* inString)
{
	std::size_t size = std::strlen(inString);
	Write(size);

	Write(inString, size * sizeof(char));
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(Entity entity)
{
	//NetworkID networkID = mLinkingContext->GetNetworkID(entity);
	//Write(networkID);
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Write(const void* inData, U32 byteCount)
{
	U32 newHead = m_head + byteCount;
	if (newHead > m_capacity)
	{
		Realloc(std::max(m_capacity * 2, newHead));
	}

	std::memcpy(m_buffer + m_head, inData, byteCount);

	m_head = newHead;
}

//----------------------------------------------------------------------------------------------------
void OutputMemoryStream::Realloc(U32 capacity)
{
	assert(capacity > m_capacity);

	m_buffer = static_cast<char*>(std::realloc(m_buffer, capacity));
	assert(m_buffer != nullptr);

	m_capacity = capacity;
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
