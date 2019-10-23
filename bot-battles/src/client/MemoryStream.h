#ifndef __MEMORY_STREAM_H__
#define __MEMORY_STREAM_H__

#include "NetworkDefs.h"
#include "ByteSwap.h"
#include "EntityDefs.h"

namespace sand
{

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
	inline U32 FLOAT_TO_FIXED(float number, float min, float precision)
	{
		return static_cast<U32>((number - min) / precision);
	}

	//----------------------------------------------------------------------------------------------------
	// Fixed point
	inline float FIXED_TO_FLOAT(U32 number, float min, float precision)
	{
		return static_cast<float>(number) * precision + min;
	}


	//----------------------------------------------------------------------------------------------------
	// Bit stream
	class OutputMemoryStream
	{
	public:
		OutputMemoryStream();
		~OutputMemoryStream();

		template <typename T>
		void Write(T inData, U32 bitCount = sizeof(T) * 8);
		void Write(bool inData);
		template <typename T>
		void Write(const std::vector<T>& inVector);
		void Write(const std::string& inString);
		void Write(const char* inString);
		void Write(const Vec2& inVec);
		void Write(Entity inEntity);
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
		void WriteBits(const void* inData, U32 bitCount);
		void WriteBits(U8 inData, U32 bitCount);
		void WriteBytes(const void* inData, U32 byteCount);
		
		void Realloc(U32 bitCapacity);

	private:
		char* m_buffer;
		U32 m_head;
		U32 m_capacity;
	};

	//----------------------------------------------------------------------------------------------------
	template<typename T>
	inline void OutputMemoryStream::Write(T inData, U32 bitCount)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Data is a non-primitive type");

		if (STREAM_ENDIANNESS == PLATFORM_ENDIANNESS())
		{
			WriteBytes(&inData, sizeof(inData));
		}
		else
		{
			T swappedData = ByteSwap(inData);
			WriteBytes(&swappedData, sizeof(swappedData));
		}
	}

	//----------------------------------------------------------------------------------------------------
	template<typename T>
	inline void OutputMemoryStream::Write(const std::vector<T>& inVector)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Data is a non-primitive type");

		std::size_t size = inVector.size();
		Write(size);

		for (const T& element : inVector)
		{
			Write(element);
		}
	}


	//----------------------------------------------------------------------------------------------------
	// Bit stream
	class InputMemoryStream
	{
	public:
		InputMemoryStream();
		~InputMemoryStream();

		template <typename T>
		void Read(T& outData, U32 bitCount = sizeof(T) * 8);
		void Read(bool& outData);
		template <typename T>
		void Read(std::vector<T>& outVector);
		void Read(std::string& outString);
		void Read(char* outString);
		void Read(Vec2& outVec);
		void Read(Entity& outEntity);
		void ReadPosition(Vec2& outVec);

		const char* GetPtr() const
		{
			return m_buffer;
		}

		void Reset();

	private:
		void ReadBits(void* outData, U32 bitCount);
		void ReadBits(U8& outData, U32 bitCount);
		void ReadBytes(void* outData, U32 byteCount);

		void Alloc(U32 bitCapacity);

	private:
		char* m_buffer;
		U32 m_head;
		U32 m_capacity;
	};

	//----------------------------------------------------------------------------------------------------
	template<typename T>
	inline void InputMemoryStream::Read(T& outData, U32 bitCount)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Data is a non-primitive type");

		if (STREAM_ENDIANNESS == PLATFORM_ENDIANNESS())
		{
			ReadBytes(&outData, sizeof(outData));
		}
		else
		{
			T unswappedData;
			ReadBytes(&unswappedData, sizeof(unswappedData));
			outData = ByteSwap(unswappedData);
		}
	}

	//----------------------------------------------------------------------------------------------------
	template<typename T>
	inline void InputMemoryStream::Read(std::vector<T>& outVector)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Data is a non-primitive type");

		std::size_t size;
		Read(size);
		outVector.resize(size);

		for (const T& element : outVector)
		{
			Read(element);
		}
	}
}

#endif
