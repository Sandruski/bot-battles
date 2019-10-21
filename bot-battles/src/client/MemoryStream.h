#ifndef __MEMORY_STREAM_H__
#define __MEMORY_STREAM_H__

#include "NetworkDefs.h"
#include "ByteSwap.h"
#include "EntityDefs.h"

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	class OutputMemoryStream
	{
	public:
		OutputMemoryStream();
		~OutputMemoryStream();

		template <typename T>
		void Write(T inData);
		template <typename T>
		void Write(const std::vector<T>& inVector);
		void Write(const std::string& inString);
		void Write(const char* inString);
		void Write(Entity entity);

		const char* GetBuffer() const
		{
			return m_buffer;
		}

		U32 GetLength() const
		{
			return m_head;
		}

	private:
		void Write(const void* inData, U32 byteCount);

		void Realloc(U32 capacity);

	private:
		char* m_buffer;
		U32 m_head;
		U32 m_capacity;
	};

	//----------------------------------------------------------------------------------------------------
	template<typename T>
	inline void OutputMemoryStream::Write(T inData)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Data is a non-primitive type");

		if (STREAM_ENDIANNESS == PLATFORM_ENDIANNESS)
		{
			Write(&inData, sizeof(inData));
		}
		else
		{
			T swappedData = ByteSwap(inData);
			Write(&swappedData, sizeof(swappedData));
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
	class InputMemoryStream
	{
	public:
		InputMemoryStream(U32 byteCount);
		~InputMemoryStream();

		template <typename T>
		void Read(T& outData);
		template <typename T>
		void Read(std::vector<T>& outVector);
		void Read(std::string& outString);
		void Read(char* outString);
		void Read(Entity& entity);

	private:
		void Read(void* outData, U32 byteCount);

	private:
		char* m_buffer;
		U32 m_head;
		U32 m_capacity;
	};

	//----------------------------------------------------------------------------------------------------
	template<typename T>
	inline void InputMemoryStream::Read(T& outData)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Data is a non-primitive type");

		if (STREAM_ENDIANNESS == PLATFORM_ENDIANNESS)
		{
			Read(&outData, sizeof(outData));
		}
		else
		{
			T unswappedData;
			Read(&unswappedData, sizeof(unswappedData));
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
