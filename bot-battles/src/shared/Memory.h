#ifndef __MEMORY_H__
#define __MEMORY_H__

typedef std::uint8_t U8;
typedef std::int8_t I8;

typedef std::uint16_t U16;
typedef std::int16_t I16;

typedef std::uint32_t U32;
typedef std::int32_t I32;

typedef std::uint64_t U64;
typedef std::int64_t I64;

typedef float F32;
typedef double F64;

template <class T>
inline void SAFE_DELETE_POINTER(T*&p)
{
	if (p != nullptr)
	{
		delete p;
		p = nullptr;
	}
}

template <class T>
inline void SAFE_DELETE_ARRAY(T*&p)
{
	if (p != nullptr)
	{
		delete[] p;
		p = nullptr;
	}
}

#endif
