#ifndef __BYTE_SWAP_H__
#define __BYTE_SWAP_H__

namespace sand
{

	//----------------------------------------------------------------------------------------------------
	inline U16 ByteSwap2(U16 inData)
	{
		return (
			((inData & 0xFF00) >> 8) 
			| ((inData & 0x00FF) << 8)
			);
	}

	//----------------------------------------------------------------------------------------------------
	inline U32 ByteSwap4(U32 inData)
	{
		return (
			((inData & 0xFF000000) >> 24)
			| ((inData & 0x00FF0000) >> 8)
			| ((inData & 0x0000FF00) << 8)
			| ((inData & 0x000000FF) << 24)
			);
	}

	//----------------------------------------------------------------------------------------------------
	inline U64 ByteSwap8(U64 inData)
	{
		return (
			((inData & 0xFF00000000000000) >> 56)
			| ((inData & 0x00FF000000000000) >> 40)
			| ((inData & 0x0000FF0000000000) >> 24)
			| ((inData & 0x000000FF00000000) >> 8)
			| ((inData & 0x00000000FF000000) << 8)
			| ((inData & 0x0000000000FF0000) << 24)
			| ((inData & 0x000000000000FF00) << 40)
			| ((inData & 0x00000000000000FF) << 56)
			);
	}


	//----------------------------------------------------------------------------------------------------
	template <typename fromType, typename toType>
	class TypeAliaser
	{
	public:
		TypeAliaser(fromType fromValue);
		toType& Get();

	public:
		union
		{
			fromType m_fromValue;
			toType m_toValue;
		};
	};

	//----------------------------------------------------------------------------------------------------
	template<typename fromType, typename toType>
	inline TypeAliaser<fromType, toType>::TypeAliaser(fromType fromValue)
		: m_fromValue(fromValue)
	{
	}

	//----------------------------------------------------------------------------------------------------
	template<typename fromType, typename toType>
	inline toType& TypeAliaser<fromType, toType>::Get()
	{
		return m_toValue;
	}


	//----------------------------------------------------------------------------------------------------
	template <typename T, std::size_t> class ByteSwapper;

	//----------------------------------------------------------------------------------------------------
	template <typename T>
	class ByteSwapper<T, 2>
	{
	public:
		T Swap(T data) const;
	};

	//----------------------------------------------------------------------------------------------------
	template<typename T>
	inline T ByteSwapper<T, 2>::Swap(T data) const
	{
		U16 U16Data = TypeAliaser<T, U16>(data).Get();
		U16 swappedU16Data = ByteSwap2(U16Data);
		T swappedData = TypeAliaser<U16, T>(swappedU16Data).Get();

		return swappedData;
	}

	//----------------------------------------------------------------------------------------------------
	template <typename T>
	class ByteSwapper<T, 4>
	{
	public:
		T Swap(T data) const;
	};

	//----------------------------------------------------------------------------------------------------
	template<typename T>
	inline T ByteSwapper<T, 4>::Swap(T data) const
	{
		U32 U32Data = TypeAliaser<T, U32>(data).Get();
		U32 swappedU32Data = ByteSwap4(U32Data);
		T swappedData = TypeAliaser<U32, T>(swappedU32Data).Get();

		return swappedData;
	}

	//----------------------------------------------------------------------------------------------------
	template <typename T>
	class ByteSwapper<T, 8>
	{
	public:
		T Swap(T data) const;
	};

	//----------------------------------------------------------------------------------------------------
	template<typename T>
	inline T ByteSwapper<T, 8>::Swap(T data) const
	{
		U64 U64Data = TypeAliaser<T, U64>(data).Get();
		U64 swappedU64Data = ByteSwap8(U64Data);
		T swappedData = TypeAliaser<U64, T>(swappedU64Data).Get();

		return swappedData;
	}


	//----------------------------------------------------------------------------------------------------
	template <typename T>
	T ByteSwap(T data)
	{
		return ByteSwapper<T, sizeof(T)>().Swap(data);
	}
}

#endif
