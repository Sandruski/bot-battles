#ifndef __NETWORK_DEFS_H__
#define __NETWORK_DEFS_H__

namespace sand {

	using NetworkID = U32;
	const NetworkID INVALID_NETWORK_ID = 0; // TODO: this should be the number of MAX_NETWORK_IDs

	enum class Endianness 
	{
		LITTLE_ENDIAN, // Intel's x86, Intel's x64, Apple's iOS
		BIG_ENDIAN // Xbox 360, PlayStation 3, IBM's PowerPC
	};

	const Endianness STREAM_ENDIANNESS = Endianness::LITTLE_ENDIAN;

	inline const Endianness PLATFORM_ENDIANNESS()
	{
		short word = 0x1;
		char* ptr = reinterpret_cast<char*>(&word);
		return ptr[0] ? Endianness::LITTLE_ENDIAN : Endianness::BIG_ENDIAN;
	}

	const U32 MAX_PACKET_SIZE = 1470;
}

#endif
